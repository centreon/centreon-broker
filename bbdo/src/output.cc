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
#include <stdint.h>
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/output.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bbdo;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Serialize an event in the BBDO protocol.
 *
 *  @param[out] data Serialized event.
 *  @param[in]  e    Event to serialize.
 */
template <typename T, unsigned int type>
static void serialize(QByteArray& data, io::data const* e) {
  T const& t(*static_cast<T const*>(e));
  unsigned int beginning(data.size());
  data.resize(data.size() + BBDO_HEADER_SIZE);
  *(static_cast<uint32_t*>(static_cast<void*>(data.data() + data.size())) - 1)
    = htonl(type);
  for (typename std::vector<getter_setter<T> >::const_iterator
         it(bbdo_mapped_type<T>::table.begin()),
         end(bbdo_mapped_type<T>::table.end());
       it != end;
       ++it) {
    // Serialization itself.
    (*it->getter)(t, *it->member, data);

    // Packet splitting.
    if (static_cast<unsigned int>(data.size())
        >= (beginning + BBDO_HEADER_SIZE + 0xFFFF)) {
      // Set size.
      *(static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning)) + 1)
        = 0xFFFF;

      // Set checksum.
      uint16_t chksum(qChecksum(
                        data.data() + beginning + 2,
                        BBDO_HEADER_SIZE - 2));
      *static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning))
        = htons(chksum);

      // Create new header.
      beginning += BBDO_HEADER_SIZE + 0xFFFF;
      char header[BBDO_HEADER_SIZE];
      memset(header, 0, sizeof(header));
      *static_cast<uint32_t*>(static_cast<void*>(header + 4))
        = htonl(type);
      data.insert(beginning, header, sizeof(header));
    }
  }

  // Set (last) packet size.
  *(static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning)) + 1)
    = htons(data.size() - beginning - BBDO_HEADER_SIZE);

  // Checksum.
  uint16_t chksum(qChecksum(
                    data.data() + beginning + 2,
                    BBDO_HEADER_SIZE - 2));
  *static_cast<uint16_t*>(static_cast<void*>(data.data() + beginning))
    = htons(chksum);

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
output::output() : _process_out(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
output::output(output const& right)
  : io::stream(right), _process_out(right._process_out) {}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
output& output::operator=(output const& right) {
  if (this != &right) {
    io::stream::operator=(right);
    _process_out = right._process_out;
  }
  return (*this);
}

/**
 *  Enable or disable output processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output processing.
 */
void output::process(bool in, bool out) {
  (void)in;
  _process_out = out;
  return ;
}

/**
 *  Read data.
 *
 *  @param[out] d Cleared.
 */
void output::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (exceptions::msg()
         << "BBDO: attempt to read from an output object");
  return ;
}

/**
 *  Get statistics.
 *
 *  @param[out] buffer Output buffer.
 */
void output::statistics(std::string& buffer) const {
  if (!_to.isNull())
    _to->statistics(buffer);
  return ;
}

/**
 *  Send an event.
 *
 *  @param[in] e Event to send.
 *
 *  @return Number of events acknowledged.
 */
unsigned int output::write(misc::shared_ptr<io::data> const& e) {
  // Redirection array.
  static struct {
    unsigned int type;
    void (*      routine)(QByteArray&, io::data const*);
  } const helpers[] = {
    { io::data::data_type(io::data::neb, neb::de_acknowledgement),
      &serialize<neb::acknowledgement, BBDO_ID(BBDO_NEB_TYPE, 1)> },
    { io::data::data_type(io::data::neb, neb::de_comment),
      &serialize<neb::comment, BBDO_ID(BBDO_NEB_TYPE, 2)> },
    { io::data::data_type(io::data::neb, neb::de_custom_variable),
      &serialize<neb::custom_variable, BBDO_ID(BBDO_NEB_TYPE, 3)> },
    { io::data::data_type(io::data::neb, neb::de_custom_variable_status),
      &serialize<neb::custom_variable_status, BBDO_ID(BBDO_NEB_TYPE, 4)> },
    { io::data::data_type(io::data::neb, neb::de_downtime),
      &serialize<neb::downtime, BBDO_ID(BBDO_NEB_TYPE, 5)> },
    { io::data::data_type(io::data::neb, neb::de_event_handler),
      &serialize<neb::event_handler, BBDO_ID(BBDO_NEB_TYPE, 6)> },
    { io::data::data_type(io::data::neb, neb::de_flapping_status),
      &serialize<neb::flapping_status, BBDO_ID(BBDO_NEB_TYPE, 7)> },
    { io::data::data_type(io::data::neb, neb::de_host),
      &serialize<neb::host, BBDO_ID(BBDO_NEB_TYPE, 8)> },
    { io::data::data_type(io::data::neb, neb::de_host_check),
      &serialize<neb::host_check, BBDO_ID(BBDO_NEB_TYPE, 9)> },
    { io::data::data_type(io::data::neb, neb::de_host_dependency),
      &serialize<neb::host_dependency, BBDO_ID(BBDO_NEB_TYPE, 10)> },
    { io::data::data_type(io::data::neb, neb::de_host_group),
      &serialize<neb::host_group, BBDO_ID(BBDO_NEB_TYPE, 11)> },
    { io::data::data_type(io::data::neb, neb::de_host_group_member),
      &serialize<neb::host_group_member, BBDO_ID(BBDO_NEB_TYPE, 12)> },
    { io::data::data_type(io::data::neb, neb::de_host_parent),
      &serialize<neb::host_parent, BBDO_ID(BBDO_NEB_TYPE, 13)> },
    { io::data::data_type(io::data::neb, neb::de_host_status),
      &serialize<neb::host_status, BBDO_ID(BBDO_NEB_TYPE, 14)> },
    { io::data::data_type(io::data::neb, neb::de_instance),
      &serialize<neb::instance, BBDO_ID(BBDO_NEB_TYPE, 15)> },
    { io::data::data_type(io::data::neb, neb::de_instance_status),
      &serialize<neb::instance_status, BBDO_ID(BBDO_NEB_TYPE, 16)> },
    { io::data::data_type(io::data::neb, neb::de_log_entry),
      &serialize<neb::log_entry, BBDO_ID(BBDO_NEB_TYPE, 17)> },
    { io::data::data_type(io::data::neb, neb::de_module),
      &serialize<neb::module, BBDO_ID(BBDO_NEB_TYPE, 18)> },
    { io::data::data_type(io::data::neb, neb::de_notification),
      &serialize<neb::notification, BBDO_ID(BBDO_NEB_TYPE, 19)> },
    { io::data::data_type(io::data::neb, neb::de_service),
      &serialize<neb::service, BBDO_ID(BBDO_NEB_TYPE, 20)> },
    { io::data::data_type(io::data::neb, neb::de_service_check),
      &serialize<neb::service_check, BBDO_ID(BBDO_NEB_TYPE, 21)> },
    { io::data::data_type(io::data::neb, neb::de_service_dependency),
      &serialize<neb::service_dependency, BBDO_ID(BBDO_NEB_TYPE, 22)> },
    { io::data::data_type(io::data::neb, neb::de_service_group),
      &serialize<neb::service_group, BBDO_ID(BBDO_NEB_TYPE, 23)> },
    { io::data::data_type(io::data::neb, neb::de_service_group_member),
      &serialize<neb::service_group_member, BBDO_ID(BBDO_NEB_TYPE, 24)> },
    { io::data::data_type(io::data::neb, neb::de_service_status),
      &serialize<neb::service_status, BBDO_ID(BBDO_NEB_TYPE, 25)> },
    { io::data::data_type(io::data::storage, storage::de_metric),
      &serialize<storage::metric, BBDO_ID(BBDO_STORAGE_TYPE, 1)> },
    { io::data::data_type(io::data::storage, storage::de_rebuild),
      &serialize<storage::rebuild, BBDO_ID(BBDO_STORAGE_TYPE, 2)> },
    { io::data::data_type(io::data::storage, storage::de_remove_graph),
      &serialize<storage::remove_graph, BBDO_ID(BBDO_STORAGE_TYPE, 3)> },
    { io::data::data_type(io::data::storage, storage::de_status),
      &serialize<storage::status, BBDO_ID(BBDO_STORAGE_TYPE, 4)> },
    { io::data::data_type(io::data::correlation, correlation::de_engine_state),
      &serialize<correlation::engine_state, BBDO_ID(BBDO_CORRELATION_TYPE, 1)> },
    { io::data::data_type(io::data::correlation, correlation::de_host_state),
      &serialize<correlation::host_state, BBDO_ID(BBDO_CORRELATION_TYPE, 2)> },
    { io::data::data_type(io::data::correlation, correlation::de_issue),
      &serialize<correlation::issue, BBDO_ID(BBDO_CORRELATION_TYPE, 3)> },
    { io::data::data_type(io::data::correlation, correlation::de_issue_parent),
      &serialize<correlation::issue_parent, BBDO_ID(BBDO_CORRELATION_TYPE, 4)> },
    { io::data::data_type(io::data::correlation, correlation::de_service_state),
      &serialize<correlation::service_state, BBDO_ID(BBDO_CORRELATION_TYPE, 5)> },
    { io::data::data_type(io::data::bbdo, bbdo::de_version_response),
      &serialize<version_response, BBDO_ID(BBDO_INTERNAL_TYPE, 1)> }
  };

  // Check if data should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, !_process_out)
           << "BBDO: output stream is shutdown");

  // Check if data exists.
  if (!e.isNull()) {
    unsigned int event_type(e->type());
    for (unsigned int i(0); i < sizeof(helpers) / sizeof(*helpers); ++i)
      if (helpers[i].type == event_type) {
        logging::debug(logging::medium)
          << "BBDO: serializing event of type '" << event_type << "'";
        misc::shared_ptr<io::raw> data(new io::raw);
        (*helpers[i].routine)(*data, &*e);
        logging::debug(logging::medium) << "BBDO: event of type '"
          << event_type << "' successfully serialized in "
          << data->size() << " bytes";
        _to->write(data.staticCast<io::data>());
        break ;
      }
  }
  else
    _to->write(e);

  return (1);
}
