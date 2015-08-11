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
#include "com/centreon/broker/bam/events.hh"
#include "com/centreon/broker/bbdo/input.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/reload.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
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
  // Redirection array.
  static struct {
    unsigned int id;
    io::data* (* routine)(char const*, unsigned int);
  } const helpers[] = {
    { BBDO_ID(BBDO_NEB_TYPE, 1),
      &unserialize<neb::acknowledgement> },
    { BBDO_ID(BBDO_NEB_TYPE, 2),
      &unserialize<neb::comment> },
    { BBDO_ID(BBDO_NEB_TYPE, 3),
      &unserialize<neb::custom_variable> },
    { BBDO_ID(BBDO_NEB_TYPE, 4),
      &unserialize<neb::custom_variable_status> },
    { BBDO_ID(BBDO_NEB_TYPE, 5),
      &unserialize<neb::downtime> },
    { BBDO_ID(BBDO_NEB_TYPE, 6),
      &unserialize<neb::event_handler> },
    { BBDO_ID(BBDO_NEB_TYPE, 7),
      &unserialize<neb::flapping_status> },
    { BBDO_ID(BBDO_NEB_TYPE, 8),
      &unserialize<neb::host> },
    { BBDO_ID(BBDO_NEB_TYPE, 9),
      &unserialize<neb::host_check> },
    { BBDO_ID(BBDO_NEB_TYPE, 10),
      &unserialize<neb::host_dependency> },
    { BBDO_ID(BBDO_NEB_TYPE, 11),
      &unserialize<neb::host_group> },
    { BBDO_ID(BBDO_NEB_TYPE, 12),
      &unserialize<neb::host_group_member> },
    { BBDO_ID(BBDO_NEB_TYPE, 13),
      &unserialize<neb::host_parent> },
    { BBDO_ID(BBDO_NEB_TYPE, 14),
      &unserialize<neb::host_status> },
    { BBDO_ID(BBDO_NEB_TYPE, 15),
      &unserialize<neb::instance> },
    { BBDO_ID(BBDO_NEB_TYPE, 16),
      &unserialize<neb::instance_status> },
    { BBDO_ID(BBDO_NEB_TYPE, 17),
      &unserialize<neb::log_entry> },
    { BBDO_ID(BBDO_NEB_TYPE, 18),
      &unserialize<neb::module> },
    { BBDO_ID(BBDO_NEB_TYPE, 19),
      &unserialize<neb::notification> },
    { BBDO_ID(BBDO_NEB_TYPE, 20),
      &unserialize<neb::service> },
    { BBDO_ID(BBDO_NEB_TYPE, 21),
      &unserialize<neb::service_check> },
    { BBDO_ID(BBDO_NEB_TYPE, 22),
      &unserialize<neb::service_dependency> },
    { BBDO_ID(BBDO_NEB_TYPE, 23),
      &unserialize<neb::service_group> },
    { BBDO_ID(BBDO_NEB_TYPE, 24),
      &unserialize<neb::service_group_member> },
    { BBDO_ID(BBDO_NEB_TYPE, 25),
      &unserialize<neb::service_status> },
    { BBDO_ID(BBDO_STORAGE_TYPE, 1),
      &unserialize<storage::metric> },
    { BBDO_ID(BBDO_STORAGE_TYPE, 2),
      &unserialize<storage::rebuild> },
    { BBDO_ID(BBDO_STORAGE_TYPE, 3),
      &unserialize<storage::remove_graph> },
    { BBDO_ID(BBDO_STORAGE_TYPE, 4),
      &unserialize<storage::status> },
    { BBDO_ID(BBDO_CORRELATION_TYPE, 1),
      &unserialize<correlation::engine_state> },
    { BBDO_ID(BBDO_CORRELATION_TYPE, 2),
      &unserialize<correlation::host_state> },
    { BBDO_ID(BBDO_CORRELATION_TYPE, 3),
      &unserialize<correlation::issue> },
    { BBDO_ID(BBDO_CORRELATION_TYPE, 4),
      &unserialize<correlation::issue_parent> },
    { BBDO_ID(BBDO_CORRELATION_TYPE, 5),
      &unserialize<correlation::service_state> },
    { BBDO_ID(BBDO_BAM_TYPE, 1),
      &unserialize<bam::ba_status> },
    { BBDO_ID(BBDO_BAM_TYPE, 2),
      &unserialize<bam::bool_status> },
    { BBDO_ID(BBDO_BAM_TYPE, 3),
      &unserialize<bam::kpi_status> },
    { BBDO_ID(BBDO_BAM_TYPE, 4),
      &unserialize<bam::meta_service_status> },
    { BBDO_ID(BBDO_BAM_TYPE, 5),
      &unserialize<bam::ba_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 6),
      &unserialize<bam::kpi_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 7),
      &unserialize<bam::ba_duration_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 8),
      &unserialize<bam::dimension_ba_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 9),
      &unserialize<bam::dimension_kpi_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 10),
      &unserialize<bam::dimension_ba_bv_relation_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 11),
      &unserialize<bam::dimension_bv_event> },
    { BBDO_ID(BBDO_BAM_TYPE, 12),
      &unserialize<bam::dimension_truncate_table_signal> },
    { BBDO_ID(BBDO_BAM_TYPE, 13),
      &unserialize<bam::rebuild> },
    { BBDO_ID(BBDO_BAM_TYPE, 14),
      &unserialize<bam::dimension_timeperiod> },
    { BBDO_ID(BBDO_BAM_TYPE, 15),
      &unserialize<bam::dimension_ba_timeperiod_relation> },
    { BBDO_ID(BBDO_INTERNAL_TYPE, 1),
      &unserialize<version_response> },
    { BBDO_ID(BBDO_DUMPER_TYPE, 1),
      &unserialize<dumper::dump> },
    { BBDO_ID(BBDO_DUMPER_TYPE, 2),
      &unserialize<dumper::reload> },
    { BBDO_ID(BBDO_DUMPER_TYPE, 3),
      &unserialize<dumper::db_dump> },
    { BBDO_ID(BBDO_DUMPER_TYPE, 4),
      &unserialize<dumper::entries::ba> },
    { BBDO_ID(BBDO_DUMPER_TYPE, 5),
      &unserialize<dumper::entries::kpi> }
  };

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
    if (!d.isNull()
        && d->type() == io::events::data_type<io::events::internal, 1>::value) {
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
