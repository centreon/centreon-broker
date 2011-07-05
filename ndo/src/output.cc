/*
** Copyright 2009-2011 Merethis
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

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/ndo/internal.hh"
#include "com/centreon/broker/ndo/output.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/status.hh"
#include "mapping.hh"
#include "nagios/protoapi.h"

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
  typename std::map<int, getter_setter<T> >::const_iterator end
    = ndo_mapped_type<T>::map.end();
  for (typename std::map<int, getter_setter<T> >::const_iterator it
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
output::output() {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
output::output(output const& o) : io::stream(o) {}

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
  io::stream::operator=(o);
  return (*this);
}

/**
 *  Read data.
 */
QSharedPointer<io::data> output::read() {
  throw (exceptions::msg() << "NDO: attempt to read from an " \
           "output object");
  return (QSharedPointer<io::data>());
}

/**
 *  Send an event.
 *
 *  @param[in] e Event to send.
 */
void output::write(QSharedPointer<io::data> e) {
  logging::debug << logging::MEDIUM << "NDO: writing data";
  std::stringstream buffer;
  if (e->type() == "com::centreon::broker::neb::acknowledgement") {
    buffer << NDO_API_ACKNOWLEDGEMENTDATA << ":\n";
    handle_event<neb::acknowledgement>(
      *static_cast<neb::acknowledgement*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::comment") {
    buffer << NDO_API_COMMENTDATA << ":\n";
    handle_event<neb::comment>(
      *static_cast<neb::comment*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::custom_variable") {
    buffer << NDO_API_RUNTIMEVARIABLES << ":\n";
    handle_event<neb::custom_variable>(
      *static_cast<neb::custom_variable*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::custom_variable_status") {
    buffer << NDO_API_CONFIGVARIABLES << ":\n";
    handle_event<neb::custom_variable_status>(
      *static_cast<neb::custom_variable_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::downtime") {
    buffer << NDO_API_DOWNTIMEDATA << ":\n";
    handle_event<neb::downtime>(
      *static_cast<neb::downtime*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::event_handler") {
    buffer << NDO_API_EVENTHANDLERDATA << ":\n";
    handle_event<neb::event_handler>(
      *static_cast<neb::event_handler*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::flapping_status") {
    buffer << NDO_API_FLAPPINGDATA << ":\n";
    handle_event<neb::flapping_status>(
      *static_cast<neb::flapping_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host") {
    buffer << NDO_API_HOSTDEFINITION << ":\n";
    handle_event<neb::host>(
      *static_cast<neb::host*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host_check") {
    buffer << NDO_API_HOSTCHECKDATA << ":\n";
    handle_event<neb::host_check>(
      *static_cast<neb::host_check*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host_dependency") {
    buffer << NDO_API_HOSTDEPENDENCYDEFINITION << ":\n";
    handle_event<neb::host_dependency>(
      *static_cast<neb::host_dependency*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host_group") {
    buffer << NDO_API_HOSTGROUPDEFINITION << ":\n";
    handle_event<neb::host_group>(
      *static_cast<neb::host_group*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host_group_member") {
    buffer << NDO_API_HOSTGROUPMEMBERDEFINITION << ":\n";
    handle_event<neb::host_group_member>(
      *static_cast<neb::host_group_member*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host_parent") {
    buffer << NDO_API_HOSTPARENT << ":\n";
    handle_event<neb::host_parent>(
      *static_cast<neb::host_parent*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::host_status") {
    buffer << NDO_API_HOSTSTATUSDATA << ":\n";
    handle_event<neb::host_status>(
      *static_cast<neb::host_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::instance") {
    buffer << NDO_API_PROCESSDATA << ":\n";
    handle_event<neb::instance>(
      *static_cast<neb::instance*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::instance_status") {
    buffer << NDO_API_PROGRAMSTATUSDATA << ":\n";
    handle_event<neb::instance_status>(
      *static_cast<neb::instance_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::log_entry") {
    buffer << NDO_API_LOGDATA << ":\n";
    handle_event<neb::log_entry>(
      *static_cast<neb::log_entry*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::module") {
    buffer << NDO_API_COMMANDDEFINITION << ":\n";
    handle_event<neb::module>(
      *static_cast<neb::module*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::notification") {
    buffer << NDO_API_NOTIFICATIONDATA << ":\n";
    handle_event<neb::notification>(
      *static_cast<neb::notification*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::service") {
    buffer << NDO_API_SERVICEDEFINITION << ":\n";
    handle_event<neb::service>(
      *static_cast<neb::service*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::service_check") {
    buffer << NDO_API_SERVICECHECKDATA << ":\n";
    handle_event<neb::service_check>(
      *static_cast<neb::service_check*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::service_dependency") {
    buffer << NDO_API_SERVICEDEPENDENCYDEFINITION << ":\n";
    handle_event<neb::service_dependency>(
      *static_cast<neb::service_dependency*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::service_group") {
    buffer << NDO_API_SERVICEGROUPDEFINITION << ":\n";
    handle_event<neb::service_group>(
      *static_cast<neb::service_group*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::service_group_member") {
    buffer << NDO_API_SERVICEGROUPMEMBERDEFINITION << ":\n";
    handle_event<neb::service_group_member>(
      *static_cast<neb::service_group_member*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::neb::service_status") {
    buffer << NDO_API_SERVICESTATUSDATA << ":\n";
    handle_event<neb::service_status>(
      *static_cast<neb::service_status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::storage::metric") {
    buffer << NDO_API_STORAGEMETRIC << ":\n";
    handle_event<storage::metric>(
      *static_cast<storage::metric*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  else if (e->type() == "com::centreon::broker::storage::status") {
    buffer << NDO_API_STORAGESTATUS << ":\n";
    handle_event<storage::status>(
      *static_cast<storage::status*>(e.data()),
      buffer);
    buffer << NDO_API_ENDDATA << "\n";
  }
  buffer << "\n";

  // Send data.
  QSharedPointer<io::raw> data(new io::raw);
  data->append(buffer.str().c_str());
  _to->write(data.staticCast<io::data>());
  logging::debug << logging::MEDIUM << "NDO: data successfully sent";

  return ;
}
