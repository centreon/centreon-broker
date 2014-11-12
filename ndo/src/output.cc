/*
** Copyright 2009-2014 Merethis
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

#include "com/centreon/broker/bam/events.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/ndo/internal.hh"
#include "com/centreon/broker/ndo/output.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/storage/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/engine/protoapi.h"
#include "mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::ndo;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Extract event parameters and send them to the data stream.
 */
template <typename T>
static void handle_event(T const& e,
                         std::stringstream& buffer) {
  typename umap<int, getter_setter<T> >::const_iterator end
    = ndo_mapped_type<T>::map.end();
  for (typename umap<int, getter_setter<T> >::const_iterator it
         = ndo_mapped_type<T>::map.begin();
       it != end;
       ++it) {
    buffer << it->first << "=";
    (it->second.getter)(e, *it->second.member, buffer);
    buffer << "\n";
  }
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
 *  @param[in] o Object to copy.
 */
output::output(output const& o)
  : io::stream(o), _process_out(o._process_out) {}

/**
 *  Destructor.
 */
output::~output() {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
output& output::operator=(output const& o) {
  if (this != &o) {
    io::stream::operator=(o);
    _process_out = o._process_out;
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
  throw (exceptions::msg() << "NDO: attempt to read from an " \
           "output object");
  return ;
}

/**
 *  Get statistics.
 *
 *  @param[out] buffer Output buffer.
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
  // Check if data exists and should be processed.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, !_process_out)
           << "NDO output stream is shutdown");
  if (e.isNull()) {
    _to->write(e);
    return (1);
  }

  logging::debug(logging::medium) << "NDO: writing data ("
    << e->type() << ")";
  std::stringstream buffer;
  if (e->type() == io::events::data_type<io::events::neb, neb::de_acknowledgement>::value) {
    buffer << NDO_API_ACKNOWLEDGEMENTDATA << ":\n";
    handle_event<neb::acknowledgement>(
      *static_cast<neb::acknowledgement*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_comment>::value) {
    buffer << NDO_API_COMMENTDATA << ":\n";
    handle_event<neb::comment>(
      *static_cast<neb::comment*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_custom_variable>::value) {
    buffer << NDO_API_RUNTIMEVARIABLES << ":\n";
    handle_event<neb::custom_variable>(
      *static_cast<neb::custom_variable*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_custom_variable_status>::value) {
    buffer << NDO_API_CONFIGVARIABLES << ":\n";
    handle_event<neb::custom_variable_status>(
      *static_cast<neb::custom_variable_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_downtime>::value) {
    buffer << NDO_API_DOWNTIMEDATA << ":\n";
    handle_event<neb::downtime>(
      *static_cast<neb::downtime*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_event_handler>::value) {
    buffer << NDO_API_EVENTHANDLERDATA << ":\n";
    handle_event<neb::event_handler>(
      *static_cast<neb::event_handler*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_flapping_status>::value) {
    buffer << NDO_API_FLAPPINGDATA << ":\n";
    handle_event<neb::flapping_status>(
      *static_cast<neb::flapping_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host>::value) {
    buffer << NDO_API_HOSTDEFINITION << ":\n";
    handle_event<neb::host>(
      *static_cast<neb::host*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host_check>::value) {
    buffer << NDO_API_HOSTCHECKDATA << ":\n";
    handle_event<neb::host_check>(
      *static_cast<neb::host_check*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host_dependency>::value) {
    buffer << NDO_API_HOSTDEPENDENCYDEFINITION << ":\n";
    handle_event<neb::host_dependency>(
      *static_cast<neb::host_dependency*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host_group>::value) {
    buffer << NDO_API_HOSTGROUPDEFINITION << ":\n";
    handle_event<neb::host_group>(
      *static_cast<neb::host_group*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host_group_member>::value) {
    buffer << NDO_API_HOSTGROUPMEMBERDEFINITION << ":\n";
    handle_event<neb::host_group_member>(
      *static_cast<neb::host_group_member*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host_parent>::value) {
    buffer << NDO_API_HOSTPARENT << ":\n";
    handle_event<neb::host_parent>(
      *static_cast<neb::host_parent*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_host_status>::value) {
    buffer << NDO_API_HOSTSTATUSDATA << ":\n";
    handle_event<neb::host_status>(
      *static_cast<neb::host_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_instance>::value) {
    buffer << NDO_API_PROCESSDATA << ":\n";
    handle_event<neb::instance>(
      *static_cast<neb::instance*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_instance_status>::value) {
    buffer << NDO_API_PROGRAMSTATUSDATA << ":\n";
    handle_event<neb::instance_status>(
      *static_cast<neb::instance_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_log_entry>::value) {
    buffer << NDO_API_LOGDATA << ":\n";
    handle_event<neb::log_entry>(
      *static_cast<neb::log_entry*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_module>::value) {
    buffer << NDO_API_COMMANDDEFINITION << ":\n";
    handle_event<neb::module>(
      *static_cast<neb::module*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_notification>::value) {
    buffer << NDO_API_NOTIFICATIONDATA << ":\n";
    handle_event<neb::notification>(
      *static_cast<neb::notification*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_service>::value) {
    buffer << NDO_API_SERVICEDEFINITION << ":\n";
    handle_event<neb::service>(
      *static_cast<neb::service*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_service_check>::value) {
    buffer << NDO_API_SERVICECHECKDATA << ":\n";
    handle_event<neb::service_check>(
      *static_cast<neb::service_check*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_service_dependency>::value) {
    buffer << NDO_API_SERVICEDEPENDENCYDEFINITION << ":\n";
    handle_event<neb::service_dependency>(
      *static_cast<neb::service_dependency*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_service_group>::value) {
    buffer << NDO_API_SERVICEGROUPDEFINITION << ":\n";
    handle_event<neb::service_group>(
      *static_cast<neb::service_group*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_service_group_member>::value) {
    buffer << NDO_API_SERVICEGROUPMEMBERDEFINITION << ":\n";
    handle_event<neb::service_group_member>(
      *static_cast<neb::service_group_member*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::neb, neb::de_service_status>::value) {
    buffer << NDO_API_SERVICESTATUSDATA << ":\n";
    handle_event<neb::service_status>(
      *static_cast<neb::service_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::storage, storage::de_metric>::value) {
    buffer << NDO_API_STORAGEMETRIC << ":\n";
    handle_event<storage::metric>(
      *static_cast<storage::metric*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::storage, storage::de_rebuild>::value) {
    buffer << NDO_API_STORAGEREBUILD << ":\n";
    handle_event<storage::rebuild>(
      *static_cast<storage::rebuild*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::storage, storage::de_remove_graph>::value) {
    buffer << NDO_API_STORAGEREMOVEGRAPH << ":\n";
    handle_event<storage::remove_graph>(
      *static_cast<storage::remove_graph*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::storage, storage::de_status>::value) {
    buffer << NDO_API_STORAGESTATUS << ":\n";
    handle_event<storage::status>(
      *static_cast<storage::status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::correlation, correlation::de_engine_state>::value) {
    buffer << NDO_API_CORRELATIONENGINESTATE << ":\n";
    handle_event<correlation::engine_state>(
      *static_cast<correlation::engine_state*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::correlation, correlation::de_host_state>::value) {
    buffer << NDO_API_CORRELATIONHOSTSTATE << ":\n";
    handle_event<correlation::host_state>(
      *static_cast<correlation::host_state*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::correlation, correlation::de_issue>::value) {
    buffer << NDO_API_CORRELATIONISSUE << ":\n";
    handle_event<correlation::issue>(
      *static_cast<correlation::issue*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::correlation, correlation::de_issue_parent>::value) {
    buffer << NDO_API_CORRELATIONISSUEPARENT << ":\n";
    handle_event<correlation::issue_parent>(
      *static_cast<correlation::issue_parent*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::correlation, correlation::de_service_state>::value) {
    buffer << NDO_API_CORRELATIONSERVICESTATE << ":\n";
    handle_event<correlation::service_state>(
      *static_cast<correlation::service_state*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_ba_status>::value) {
    buffer << NDO_API_BAMBASTATUS << ":\n";
    handle_event<bam::ba_status>(
      *static_cast<bam::ba_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_bool_status>::value) {
    buffer << NDO_API_BAMBOOLSTATUS << ":\n";
    handle_event<bam::bool_status>(
      *static_cast<bam::bool_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_kpi_status>::value) {
    buffer << NDO_API_BAMKPISTATUS << ":\n";
    handle_event<bam::kpi_status>(
      *static_cast<bam::kpi_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_meta_service_status>::value) {
    buffer << NDO_API_BAMMETASERVICESTATUS << ":\n";
    handle_event<bam::meta_service_status>(
      *static_cast<bam::meta_service_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_ba_event>::value) {
    buffer << NDO_API_BAMBAEVENT << ":\n";
    handle_event<bam::ba_event>(
      *static_cast<bam::ba_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_kpi_event>::value) {
    buffer << NDO_API_BAMKPIEVENT << ":\n";
    handle_event<bam::kpi_event>(
      *static_cast<bam::kpi_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_ba_duration_event>::value) {
    buffer << NDO_API_BAMBADURATIONEVENT << ":\n";
    handle_event<bam::ba_duration_event>(
      *static_cast<bam::ba_duration_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_ba_event>::value) {
    buffer << NDO_API_BAMDIMENSIONBAEVENT << ":\n";
    handle_event<bam::dimension_ba_event>(
      *static_cast<bam::dimension_ba_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_kpi_event>::value) {
    buffer << NDO_API_BAMDIMENSIONKPIEVENT << ":\n";
    handle_event<bam::dimension_kpi_event>(
      *static_cast<bam::dimension_kpi_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_ba_bv_relation_event>::value) {
    buffer << NDO_API_BAMDIMENSIONBABVRELATIONEVENT << ":\n";
    handle_event<bam::dimension_ba_bv_relation_event>(
      *static_cast<bam::dimension_ba_bv_relation_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_bv_event>::value) {
    buffer << NDO_API_BAMDIMENSIONBVEVENT << ":\n";
    handle_event<bam::dimension_bv_event>(
      *static_cast<bam::dimension_bv_event*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_truncate_table_signal>::value) {
    buffer << NDO_API_BAMDIMENSIONTRUNCATETABLESIGNAL << ":\n";
    handle_event<bam::dimension_truncate_table_signal>(
      *static_cast<bam::dimension_truncate_table_signal*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_rebuild>::value) {
    buffer << NDO_API_BAMREBUILD << ":\n";
    handle_event<bam::rebuild>(
      *static_cast<bam::rebuild*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_timeperiod>::value) {
    buffer << NDO_API_BAMDIMENSIONTIMEPERIOD << ":\n";
    handle_event<bam::dimension_timeperiod>(
      *static_cast<bam::dimension_timeperiod*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == io::events::data_type<io::events::bam, bam::de_dimension_ba_timeperiod_relation>::value) {
    buffer << NDO_API_BAMDIMENSIONBATIMEPERIODRELATION << ":\n";
    handle_event<bam::dimension_ba_timeperiod_relation>(
      *static_cast<bam::dimension_ba_timeperiod_relation*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  buffer << "\n";

  // Send data.
  misc::shared_ptr<io::raw> data(new io::raw);
  data->append(buffer.str().c_str());
  _to->write(data);
  logging::debug(logging::medium) << "NDO: data successfully sent";

  return (1);
}
