/*
** Copyright 2013 Merethis
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
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/events.hh"

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
 *
 *  @return Event.
 */
template <typename T>
static io::data* unserialize(char const* buffer, unsigned int size) {
  std::auto_ptr<T> t(new T);
  for (typename std::vector<getter_setter<T> >::const_iterator
         it(bbdo_mapped_type<T>::table.begin()),
         end(bbdo_mapped_type<T>::table.end());
       it != end;
       ++it) {
    unsigned int processed((*it->setter)(
                             *t,
                             *it->member,
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
 *  @param[in] right Object to copy.
 */
input::input(input const& right) : io::stream(right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
input::~input() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
input& input::operator=(input const& right) {
  if (this != &right) {
    io::stream::operator=(right);
    _internal_copy(right);
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
  // Redirection array.
  static struct {
    unsigned int id;
    io::data* (* routine)(char const*, unsigned int);
  } const helpers[] = {
    { 1, &unserialize<neb::acknowledgement> },
    { 2, &unserialize<neb::comment> },
    { 3, &unserialize<neb::custom_variable> },
    { 4, &unserialize<neb::custom_variable_status> },
    { 5, &unserialize<neb::downtime> },
    { 6, &unserialize<neb::event_handler> },
    { 7, &unserialize<neb::flapping_status> },
    { 8, &unserialize<neb::host> },
    { 9, &unserialize<neb::host_check> },
    { 10, &unserialize<neb::host_dependency> },
    { 11, &unserialize<neb::host_group> },
    { 12, &unserialize<neb::host_group_member> },
    { 13, &unserialize<neb::host_parent> },
    { 14, &unserialize<neb::host_status> },
    { 15, &unserialize<neb::instance> },
    { 16, &unserialize<neb::instance_status> },
    { 17, &unserialize<neb::log_entry> },
    { 18, &unserialize<neb::module> },
    { 19, &unserialize<neb::notification> },
    { 20, &unserialize<neb::service> },
    { 21, &unserialize<neb::service_check> },
    { 22, &unserialize<neb::service_dependency> },
    { 23, &unserialize<neb::service_group> },
    { 24, &unserialize<neb::service_group_member> },
    { 25, &unserialize<neb::service_status> },
    { 26, &unserialize<storage::metric> },
    { 27, &unserialize<storage::rebuild> },
    { 28, &unserialize<storage::remove_graph> },
    { 29, &unserialize<storage::status> },
    { 30, &unserialize<correlation::engine_state> },
    { 31, &unserialize<correlation::host_state> },
    { 32, &unserialize<correlation::issue> },
    { 33, &unserialize<correlation::issue_parent> },
    { 34, &unserialize<correlation::service_state> }
  };

  // Return value.
  std::auto_ptr<io::data> e;
  d.clear();

  // Read next packet header.
  _buffer_must_have_unprocessed(BBDO_HEADER_SIZE);

  // Packet size.
  unsigned int packet_size(
                 ntohs(*static_cast<uint16_t const*>(
                          static_cast<void const*>(
                            _buffer.c_str() + _processed + 2))));
  // Get event ID.
  unsigned int event_id(
                 ntohl(*static_cast<uint32_t const*>(
                          static_cast<void const*>(
                            _buffer.c_str() + _processed + 4))));
  logging::debug(logging::high)
    << "BBDO: got new header with a size of " << packet_size
    << " and an ID of " << event_id;

  // Read data payload.
  _processed += BBDO_HEADER_SIZE;
  _buffer_must_have_unprocessed(packet_size);

  // Regroup packets.
  unsigned int total_size(0);
  while (packet_size == 0xFFFF) {
    // Previous packet has been processed.
    total_size += packet_size;

    // Expect new BBDO header.
    _buffer_must_have_unprocessed(total_size + BBDO_HEADER_SIZE);

    // Next packet size.
    packet_size
      = ntohs(*static_cast<uint16_t const*>(
                 static_cast<void const*>(
                   _buffer.c_str() + _processed + total_size + 2)));

    // Remove header to regroup data payloads.
    _buffer.erase(_processed + total_size, BBDO_HEADER_SIZE);

    // Expect data payload.
    _buffer_must_have_unprocessed(total_size + packet_size);
  }
  total_size += packet_size;

  // Find routine.
  for (unsigned int i(0); i < sizeof(helpers) / sizeof(*helpers); ++i)
    if (helpers[i].id == event_id) {
      d = (*helpers[i].routine)(
            _buffer.c_str() + _processed,
            total_size);
      break ;
    }

  // Mark data as processed.
  logging::debug(logging::medium) << "BBDO: unserialized "
    << total_size + BBDO_HEADER_SIZE << " bytes";
  _processed += total_size;

  return ;
}

/**
 *  Write data.
 *
 *  @param[in] d Object to copy.
 */
void input::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (exceptions::msg()
         << "BBDO: attempt to write to an input object");
  return ;
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
void input::_buffer_must_have_unprocessed(unsigned int bytes) {
  static QString const raw_type("com::centreon::broker::io::raw");
  if (_buffer.size() < (_processed + bytes)) {
    _buffer.erase(0, _processed);
    _processed = 0;
  }
  while (_buffer.size() < (_processed + bytes)) {
    misc::shared_ptr<io::data> d;
    _from->read(d);
    if (!d.isNull()
        && d->type() == raw_type) {
      misc::shared_ptr<io::raw> r(d.staticCast<io::raw>());
      _buffer.append(r->QByteArray::data(), r->size());
    }
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
