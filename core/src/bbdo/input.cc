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
#include <cstdlib>
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
 *  Set a boolean within an object.
 */
static unsigned int set_boolean(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (!size)
    throw (exceptions::msg() << "BBDO: cannot extract boolean value: "
           << "0 bytes left in packet");
  member.set_bool(t, *static_cast<char const*>(data));
  return (1);
}

/**
 *  Set a double within an object.
 */
static unsigned int set_double(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  char const* str(static_cast<char const*>(data));
  unsigned int len(strlen(str));
  if (len >= size)
    throw (exceptions::msg() << "BBDO: cannot extract double value: "
           << "not terminating '\0' in remaining " << size
           << " bytes of packet");
  member.set_double(t, strtod(str, NULL));
  return (len + 1);
}

/**
 *  Set an integer within an object.
 */
static unsigned int set_integer(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint32_t))
    throw (exceptions::msg() << "BBDO: cannot extract integer value: "
           << size << " bytes left in packet");
  member.set_int(t, ntohl(*static_cast<uint32_t const*>(data)));
  return (sizeof(uint32_t));
}

/**
 *  Set a short within an object.
 */
static unsigned int set_short(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint16_t))
    throw (exceptions::msg() << "BBDO: cannot extract short value: "
           << size << " bytes left in packet");
  member.set_short(t, ntohs(*static_cast<uint16_t const*>(data)));
  return (sizeof(uint16_t));
}

/**
 *  Set a string within an object.
 */
static unsigned int set_string(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  char const* str(static_cast<char const*>(data));
  unsigned int len(strlen(str));
  if (len >= size)
    throw (exceptions::msg() << "BBDO: cannot extract string value: "
           << "no terminating '\0' in remaining " << size
           << " bytes of packet");
  member.set_string(t, str);
  return (len + 1);
}

/**
 *  Set a timestamp within an object.
 */
static unsigned int set_timestamp(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < 2 * sizeof(uint32_t))
    throw (exceptions::msg()
           << "BBDO: cannot extract timestamp value: "
           << size << " bytes left in packet");
  uint32_t const* ptr(static_cast<uint32_t const*>(data));
  uint64_t val(ntohl(*ptr));
  ++ptr;
  val <<= 32;
  val |= ntohl(*ptr);
  member.set_time(t, val);
  return (2 * sizeof(uint32_t));
}

/**
 *  Set an unsigned integer within an object.
 */
static unsigned int set_uint(
                      io::data& t,
                      mapping::entry const& member,
                      void const* data,
                      unsigned int size) {
  if (size < sizeof(uint32_t))
    throw (exceptions::msg()
           << "BBDO: cannot extract unsigned integer value: "
           << size << " bytes left in packet");
  member.set_uint(t, ntohl(*static_cast<uint32_t const*>(data)));
  return (sizeof(uint32_t));
}

/**
 *  Unserialize an event in the BBDO protocol.
 *
 *  @param[in] event_type  Event type.
 *  @param[in] buffer      Serialized data.
 *  @param[in] size        Buffer size.
 *
 *  @return Event.
 */
static io::data* unserialize(
                   unsigned int event_type,
                   char const* buffer,
                   unsigned int size) {
  // Get event info (operations and mapping).
  io::event_info const*
    info(io::events::instance().get_event_info(event_type));
  if (info) {
    // Create object.
    std::auto_ptr<io::data> t(info->get_operations().constructor());
    if (t.get()) {
      // Browse all mapping to unserialize the object.
      for (mapping::entry const* current_entry(info->get_mapping());
           current_entry->get_type() != mapping::source::UNKNOWN;
           ++current_entry)
        // Skip 0 numbered entries.
        if (current_entry->get_number()) {
          unsigned int rb;
          switch (current_entry->get_type()) {
          case mapping::source::BOOL:
            rb = set_boolean(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::DOUBLE:
            rb = set_double(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::INT:
            rb = set_integer(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::SHORT:
            rb = set_short(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::STRING:
            rb = set_string(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::TIME:
            rb = set_timestamp(*t, *current_entry, buffer, size);
            break ;
          case mapping::source::UINT:
            rb = set_uint(*t, *current_entry, buffer, size);
            break ;
          default:
            throw (exceptions::msg() << "BBDO: invalid mapping for "
                   << "object of type '" << info->get_name() << "': "
                   << current_entry->get_type()
                   << " is not a known type ID");
          }
          buffer += rb;
          size -= rb;
        }
      return (t.release());
    }
    else
      throw (exceptions::msg() << "BBDO: cannot create object of ID "
             << event_type << " whereas it has been registered");
  }
  else
    logging::info(logging::high)
      << "BBDO: cannot unserialize event of ID " << event_type
      << ": event was not registered and will therefore be ignored";

  return (NULL);
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

  // Unserialize event.
  d = unserialize(
        event_id,
        _buffer.c_str() + _processed,
        total_size);
  if (!d.isNull())
    logging::debug(logging::medium) << "BBDO: unserialized "
      << total_size + BBDO_HEADER_SIZE << " bytes for event of type "
      << event_id;
  else {
    logging::error(logging::medium)
      << "BBDO: unknown event type " << event_id
      << ": event cannot be decoded";
    logging::debug(logging::medium) << "BBDO: discarded "
      << total_size + BBDO_HEADER_SIZE << " bytes";
  }

  // Mark data as processed.
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
