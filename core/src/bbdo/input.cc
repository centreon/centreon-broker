/*
** Copyright 2013,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <arpa/inet.h>
#include <memory>
#include <stdint.h>
#include "com/centreon/broker/bbdo/input.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Unserialize an event in the BBDO protocol.
 *
 *  @param[in] buffer Serialized data.
 *  @param[in] size   Buffer size.
 *  @param[in] info   Event informations.
 *
 *  @return Event.
 */
static io::data* unserialize(
                   char const* buffer,
                   unsigned int size,
                   bbdo_mapped_type const& mapping_info) {
  std::auto_ptr<io::data>
    t(mapping_info.mapped_type->get_operations().constructor());
  mapping::entry const*
    current_entry(mapping_info.mapped_type->get_mapping());
  for (std::vector<getter_setter>::const_iterator
         it(mapping_info.bbdo_entries.begin()),
         end(mapping_info.bbdo_entries.end());
       it != end;
       ++it, ++current_entry) {
    // Skip 0 numbered entries.
    for (; !current_entry->is_null() && current_entry->get_number() == 0;
         ++current_entry);

    unsigned int processed((*it->setter)(
                             *t,
                             *current_entry,
                             buffer,
                             size));
    size -= processed;
    buffer += processed;
  }
  return (t.release());
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
input::input() : _process_in(true), _processed(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
input::input(input const& other) : io::stream(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
input::~input() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
input& input::operator=(input const& other) {
  if (this != &other) {
    io::stream::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Enable or disable input processing.
 *
 *  @param[in] in  Set to true to enable input processing.
 *  @param[in] out Unused.
 */
void input::process(bool in, bool out) {
  (void)out;
  _process_in = in;
  return ;
}


/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @param[out] d Next available event, NULL if stream is closed.
 */
void input::read(misc::shared_ptr<io::data>& d) {
  // Read event.
  unsigned int event_id(read_any(d));
  while (event_id
         && !d.isNull()
         && ((event_id >> 16) == BBDO_INTERNAL_TYPE)) {
    // Version response.
    if ((event_id & 0xFFFF) == 1) {
      misc::shared_ptr<version_response>
        version(d.staticCast<version_response>());
      if (version->bbdo_major != BBDO_VERSION_MAJOR)
        throw (exceptions::msg()
               << "BBDO: peer is using protocol version "
               << version->bbdo_major << "." << version->bbdo_minor
               << "." << version->bbdo_patch
               << " whereas we're using protocol version "
               << BBDO_VERSION_MAJOR << "." << BBDO_VERSION_MINOR << "."
               << BBDO_VERSION_PATCH);
      logging::info(logging::medium)
        << "BBDO: peer is using protocol version " << version->bbdo_major
        << "." << version->bbdo_minor << "." << version->bbdo_patch
        << ", we're using version " << BBDO_VERSION_MAJOR << "."
        << BBDO_VERSION_MINOR << "." << BBDO_VERSION_PATCH;
    }

    // Control messages.
    logging::debug(logging::medium) << "BBDO: event with ID "
      << event_id << " was a control message, launching recursive read";
    event_id = this->read_any(d);
  }
  return ;
}

/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @param[out] d       Next available event, NULL if stream is closed.
 *  @param[in]  timeout Timeout.
 *
 *  @return Event ID.
 */
unsigned int input::read_any(
                      misc::shared_ptr<io::data>& d,
                      time_t timeout) {
  // Return value.
  std::auto_ptr<io::data> e;
  d.clear();

  // Get header informations.
  unsigned int event_id;
  unsigned int packet_size;
  while (1) {
    // Read next packet header.
    _buffer_must_have_unprocessed(BBDO_HEADER_SIZE, timeout);

    // Packet size.
    packet_size = ntohs(*static_cast<uint16_t const*>(
                           static_cast<void const*>(
                             _buffer.c_str() + _processed + 2)));

    // Get event ID.
    event_id = ntohl(*static_cast<uint32_t const*>(
                        static_cast<void const*>(
                          _buffer.c_str() + _processed + 4)));

    // Get checksum.
    unsigned chksum(ntohs(*static_cast<uint16_t const*>(
                             static_cast<void const*>(
                               _buffer.c_str() + _processed))));

    // Check header integrity.
    uint16_t expected(qChecksum(_buffer.c_str() + _processed + 2, 6));
    if (chksum == expected)
      break ;

    // Mark data as processed.
    logging::debug(logging::low)
      << "BBDO: header integrity check failed (got " << chksum
      << ", computed " << expected << ")";
    ++_processed;

    // Timeout check.
    if ((timeout != (time_t)-1) && (time(NULL) > timeout))
      throw (exceptions::msg() << "BBDO: connection timeout");
  }

  // Log.
  logging::debug(logging::high)
    << "BBDO: got new header with a size of " << packet_size
    << " and an ID of " << event_id;

  // Read data payload.
  _processed += BBDO_HEADER_SIZE;
  _buffer_must_have_unprocessed(packet_size, timeout);

  // Regroup packets.
  unsigned int total_size(0);
  while (packet_size == 0xFFFF) {
    // Previous packet has been processed.
    total_size += packet_size;

    // Expect new BBDO header.
    _buffer_must_have_unprocessed(
      total_size + BBDO_HEADER_SIZE,
      timeout);

    // Next packet size.
    packet_size
      = ntohs(*static_cast<uint16_t const*>(
                 static_cast<void const*>(
                   _buffer.c_str() + _processed + total_size + 2)));

    // Remove header to regroup data payloads.
    _buffer.erase(_processed + total_size, BBDO_HEADER_SIZE);

    // Expect data payload.
    _buffer_must_have_unprocessed(total_size + packet_size, timeout);
  }
  total_size += packet_size;

  // Find routine.
  umap<unsigned int, bbdo_mapped_type>::const_iterator
    mapped_type(bbdo_mapping.find(event_id));
  if (mapped_type != bbdo_mapping.end()) {
    d = unserialize(
          _buffer.c_str() + _processed,
          total_size,
          mapped_type->second);
  }
  else {
    logging::debug(logging::medium)
      << "BBDO: got unknown event type " << event_id
      << ": recreating mappings and retrying";
    create_mappings();
    mapped_type = bbdo_mapping.find(event_id);
    if (mapped_type != bbdo_mapping.end())
      d = unserialize(
            _buffer.c_str() + _processed,
            total_size,
            mapped_type->second);
  }

  // Mark data as processed.
  logging::debug(logging::medium) << "BBDO: unserialized "
    << total_size + BBDO_HEADER_SIZE << " bytes";
  _processed += total_size;

  return (event_id);
}

/**
 *  Write data.
 *
 *  @param[in] d Object to copy.
 *
 *  @return Does not return, throw an exception.
 */
unsigned int input::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (exceptions::msg()
         << "BBDO: attempt to write to an input object");
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Expect buffer to have a minimal size.
 *
 *  @param[in] bytes Number of minimal buffer size.
 */
void input::_buffer_must_have_unprocessed(
              unsigned int bytes,
              time_t timeout) {
  if (_buffer.size() < (_processed + bytes)) {
    _buffer.erase(0, _processed);
    _processed = 0;
  }
  while (_buffer.size() < (_processed + bytes)) {
    misc::shared_ptr<io::data> d;
    _from->read(d);
    if (!d.isNull() && d->type() == io::raw::static_type()) {
      misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
      _buffer.append(r->QByteArray::data(), r->size());
    }
    if ((_buffer.size() < (_processed + bytes))
        && (timeout != (time_t)-1)
        && (time(NULL) > timeout))
      throw (exceptions::msg() << "BBDO: connection timeout");
  }
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void input::_internal_copy(input const& right) {
  _buffer = right._buffer;
  _process_in = right._process_in;
  _processed = right._processed;
  return ;
}
