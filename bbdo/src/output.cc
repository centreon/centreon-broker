/*
** Copyright 2013-2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <arpa/inet.h>
#include <stdint.h>
#include "com/centreon/broker/bam/events.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bbdo/internal.hh"
#include "com/centreon/broker/bbdo/output.hh"
#include "com/centreon/broker/bbdo/version_response.hh"
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/dumper/db_dump.hh"
#include "com/centreon/broker/dumper/dump.hh"
#include "com/centreon/broker/dumper/db_dump_committed.hh"
#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
#include "com/centreon/broker/dumper/entries/boolean.hh"
#include "com/centreon/broker/dumper/reload.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
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
    while (static_cast<unsigned int>(data.size())
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
 *  @param[out] tree Output tree.
 */
void output::statistics(io::properties& tree) const {
  if (!_to.isNull())
    _to->statistics(tree);
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
    { io::events::data_type<io::events::neb, neb::de_acknowledgement>::value,
      &serialize<neb::acknowledgement, BBDO_ID(BBDO_NEB_TYPE, 1)> },
    { io::events::data_type<io::events::neb, neb::de_comment>::value,
      &serialize<neb::comment, BBDO_ID(BBDO_NEB_TYPE, 2)> },
    { io::events::data_type<io::events::neb, neb::de_custom_variable>::value,
      &serialize<neb::custom_variable, BBDO_ID(BBDO_NEB_TYPE, 3)> },
    { io::events::data_type<io::events::neb, neb::de_custom_variable_status>::value,
      &serialize<neb::custom_variable_status, BBDO_ID(BBDO_NEB_TYPE, 4)> },
    { io::events::data_type<io::events::neb, neb::de_downtime>::value,
      &serialize<neb::downtime, BBDO_ID(BBDO_NEB_TYPE, 5)> },
    { io::events::data_type<io::events::neb, neb::de_event_handler>::value,
      &serialize<neb::event_handler, BBDO_ID(BBDO_NEB_TYPE, 6)> },
    { io::events::data_type<io::events::neb, neb::de_flapping_status>::value,
      &serialize<neb::flapping_status, BBDO_ID(BBDO_NEB_TYPE, 7)> },
    { io::events::data_type<io::events::neb, neb::de_host>::value,
      &serialize<neb::host, BBDO_ID(BBDO_NEB_TYPE, 8)> },
    { io::events::data_type<io::events::neb, neb::de_host_check>::value,
      &serialize<neb::host_check, BBDO_ID(BBDO_NEB_TYPE, 9)> },
    { io::events::data_type<io::events::neb, neb::de_host_dependency>::value,
      &serialize<neb::host_dependency, BBDO_ID(BBDO_NEB_TYPE, 10)> },
    { io::events::data_type<io::events::neb, neb::de_host_group>::value,
      &serialize<neb::host_group, BBDO_ID(BBDO_NEB_TYPE, 11)> },
    { io::events::data_type<io::events::neb, neb::de_host_group_member>::value,
      &serialize<neb::host_group_member, BBDO_ID(BBDO_NEB_TYPE, 12)> },
    { io::events::data_type<io::events::neb, neb::de_host_parent>::value,
      &serialize<neb::host_parent, BBDO_ID(BBDO_NEB_TYPE, 13)> },
    { io::events::data_type<io::events::neb, neb::de_host_status>::value,
      &serialize<neb::host_status, BBDO_ID(BBDO_NEB_TYPE, 14)> },
    { io::events::data_type<io::events::neb, neb::de_instance>::value,
      &serialize<neb::instance, BBDO_ID(BBDO_NEB_TYPE, 15)> },
    { io::events::data_type<io::events::neb, neb::de_instance_status>::value,
      &serialize<neb::instance_status, BBDO_ID(BBDO_NEB_TYPE, 16)> },
    { io::events::data_type<io::events::neb, neb::de_log_entry>::value,
      &serialize<neb::log_entry, BBDO_ID(BBDO_NEB_TYPE, 17)> },
    { io::events::data_type<io::events::neb, neb::de_module>::value,
      &serialize<neb::module, BBDO_ID(BBDO_NEB_TYPE, 18)> },
    { io::events::data_type<io::events::neb, neb::de_notification>::value,
      &serialize<neb::notification, BBDO_ID(BBDO_NEB_TYPE, 19)> },
    { io::events::data_type<io::events::neb, neb::de_service>::value,
      &serialize<neb::service, BBDO_ID(BBDO_NEB_TYPE, 20)> },
    { io::events::data_type<io::events::neb, neb::de_service_check>::value,
      &serialize<neb::service_check, BBDO_ID(BBDO_NEB_TYPE, 21)> },
    { io::events::data_type<io::events::neb, neb::de_service_dependency>::value,
      &serialize<neb::service_dependency, BBDO_ID(BBDO_NEB_TYPE, 22)> },
    { io::events::data_type<io::events::neb, neb::de_service_group>::value,
      &serialize<neb::service_group, BBDO_ID(BBDO_NEB_TYPE, 23)> },
    { io::events::data_type<io::events::neb, neb::de_service_group_member>::value,
      &serialize<neb::service_group_member, BBDO_ID(BBDO_NEB_TYPE, 24)> },
    { io::events::data_type<io::events::neb, neb::de_service_status>::value,
      &serialize<neb::service_status, BBDO_ID(BBDO_NEB_TYPE, 25)> },
    { io::events::data_type<io::events::neb, neb::de_instance_configuration>::value,
      &serialize<neb::instance_configuration, BBDO_ID(BBDO_NEB_TYPE, 26)> },
    { io::events::data_type<io::events::storage, storage::de_metric>::value,
      &serialize<storage::metric, BBDO_ID(BBDO_STORAGE_TYPE, 1)> },
    { io::events::data_type<io::events::storage, storage::de_rebuild>::value,
      &serialize<storage::rebuild, BBDO_ID(BBDO_STORAGE_TYPE, 2)> },
    { io::events::data_type<io::events::storage, storage::de_remove_graph>::value,
      &serialize<storage::remove_graph, BBDO_ID(BBDO_STORAGE_TYPE, 3)> },
    { io::events::data_type<io::events::storage, storage::de_status>::value,
      &serialize<storage::status, BBDO_ID(BBDO_STORAGE_TYPE, 4)> },
    { io::events::data_type<io::events::correlation, correlation::de_engine_state>::value,
      &serialize<correlation::engine_state, BBDO_ID(BBDO_CORRELATION_TYPE, 1)> },
    { io::events::data_type<io::events::correlation, correlation::de_host_state>::value,
      &serialize<correlation::host_state, BBDO_ID(BBDO_CORRELATION_TYPE, 2)> },
    { io::events::data_type<io::events::correlation, correlation::de_issue>::value,
      &serialize<correlation::issue, BBDO_ID(BBDO_CORRELATION_TYPE, 3)> },
    { io::events::data_type<io::events::correlation, correlation::de_issue_parent>::value,
      &serialize<correlation::issue_parent, BBDO_ID(BBDO_CORRELATION_TYPE, 4)> },
    { io::events::data_type<io::events::correlation, correlation::de_service_state>::value,
      &serialize<correlation::service_state, BBDO_ID(BBDO_CORRELATION_TYPE, 5)> },
    { io::events::data_type<io::events::bam, bam::de_ba_status>::value,
      &serialize<bam::ba_status, BBDO_ID(BBDO_BAM_TYPE, 1)> },
    { io::events::data_type<io::events::bam, bam::de_bool_status>::value,
      &serialize<bam::bool_status, BBDO_ID(BBDO_BAM_TYPE, 2)> },
    { io::events::data_type<io::events::bam, bam::de_kpi_status>::value,
      &serialize<bam::kpi_status, BBDO_ID(BBDO_BAM_TYPE, 3)> },
    { io::events::data_type<io::events::bam, bam::de_meta_service_status>::value,
      &serialize<bam::meta_service_status, BBDO_ID(BBDO_BAM_TYPE, 4)> },
    { io::events::data_type<io::events::bam, bam::de_ba_event>::value,
      &serialize<bam::ba_event, BBDO_ID(BBDO_BAM_TYPE, 5)> },
    { io::events::data_type<io::events::bam, bam::de_kpi_event>::value,
      &serialize<bam::kpi_event, BBDO_ID(BBDO_BAM_TYPE, 6)> },
    { io::events::data_type<io::events::bam, bam::de_ba_duration_event>::value,
      &serialize<bam::ba_duration_event, BBDO_ID(BBDO_BAM_TYPE, 7)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_ba_event>::value,
      &serialize<bam::dimension_ba_event, BBDO_ID(BBDO_BAM_TYPE, 8)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_kpi_event>::value,
      &serialize<bam::dimension_kpi_event, BBDO_ID(BBDO_BAM_TYPE, 9)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_ba_bv_relation_event>::value,
      &serialize<bam::dimension_ba_bv_relation_event, BBDO_ID(BBDO_BAM_TYPE, 10)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_bv_event>::value,
      &serialize<bam::dimension_bv_event, BBDO_ID(BBDO_BAM_TYPE, 11)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_truncate_table_signal>::value,
      &serialize<bam::dimension_truncate_table_signal, BBDO_ID(BBDO_BAM_TYPE, 12)> },
    { io::events::data_type<io::events::bam, bam::de_rebuild>::value,
      &serialize<bam::rebuild, BBDO_ID(BBDO_BAM_TYPE, 13)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_timeperiod>::value,
      &serialize<bam::dimension_timeperiod, BBDO_ID(BBDO_BAM_TYPE, 14)> },
    { io::events::data_type<io::events::bam, bam::de_dimension_ba_timeperiod_relation>::value,
      &serialize<bam::dimension_ba_timeperiod_relation, BBDO_ID(BBDO_BAM_TYPE, 15)> },
    { io::events::data_type<io::events::bbdo, bbdo::de_version_response>::value,
      &serialize<version_response, BBDO_ID(BBDO_INTERNAL_TYPE, 1)> },
    { io::events::data_type<io::events::dumper, dumper::de_dump>::value,
      &serialize<dumper::dump, BBDO_ID(BBDO_DUMPER_TYPE, 1)> },
    { io::events::data_type<io::events::dumper, dumper::de_reload>::value,
      &serialize<dumper::reload, BBDO_ID(BBDO_DUMPER_TYPE, 2)> },
    { io::events::data_type<io::events::dumper, dumper::de_db_dump>::value,
      &serialize<dumper::db_dump, BBDO_ID(BBDO_DUMPER_TYPE, 3)> },
    { io::events::data_type<io::events::dumper, dumper::de_entries_ba>::value,
      &serialize<dumper::entries::ba, BBDO_ID(BBDO_DUMPER_TYPE, 4)> },
    { io::events::data_type<io::events::dumper, dumper::de_entries_kpi>::value,
      &serialize<dumper::entries::kpi, BBDO_ID(BBDO_DUMPER_TYPE, 5)> },
    { io::events::data_type<io::events::dumper, dumper::de_entries_host>::value,
      &serialize<dumper::entries::host, BBDO_ID(BBDO_DUMPER_TYPE, 6)> },
    { io::events::data_type<io::events::dumper, dumper::de_entries_service>::value,
      &serialize<dumper::entries::service, BBDO_ID(BBDO_DUMPER_TYPE, 7)> },
    { io::events::data_type<io::events::dumper, dumper::de_entries_boolean>::value,
      &serialize<dumper::entries::boolean, BBDO_ID(BBDO_DUMPER_TYPE, 8)> },
    { io::events::data_type<io::events::dumper, dumper::de_db_dump_committed>::value,
      &serialize<dumper::db_dump_committed, BBDO_ID(BBDO_DUMPER_TYPE, 9)> }
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
        _to->write(data);
        break ;
      }
  }
  else
    _to->write(e);

  return (1);
}
