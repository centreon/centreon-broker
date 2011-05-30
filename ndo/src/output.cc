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

#include "events/events.hh"
#include "exceptions/basic.hh"
#include "mapping.hh"
#include "nagios/protoapi.h"
#include "ndo/internal.hh"
#include "ndo/output.hh"

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
output::output(output const& o) : io::ostream(o), serialization::oserial(o) {}

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
  serialization::oserial::operator=(o);
  return (*this);
}

/**
 *  Write data.
 */
unsigned int output::write(void const* data, unsigned size) {
  (void)data;
  (void)size;
  throw (exceptions::basic() << "attemp to write from a NDO output stream");
  return (0);
}

/**
 *  Send an event.
 *
 *  @param[in] e Event to send.
 */
void output::serialize(QSharedPointer<events::event> e) {
  try {
    std::stringstream buffer;
    switch (e->get_type()) {
     case events::event::ACKNOWLEDGEMENT:
      buffer << NDO_API_ACKNOWLEDGEMENTDATA << ":\n";
      handle_event<events::acknowledgement>(
        *static_cast<events::acknowledgement*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::COMMENT:
      buffer << NDO_API_COMMENTDATA << ":\n";
      handle_event<events::comment>(
        *static_cast<events::comment*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::CUSTOMVARIABLE:
      buffer << NDO_API_RUNTIMEVARIABLES << ":\n";
      handle_event<events::custom_variable>(
        *static_cast<events::custom_variable*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::CUSTOMVARIABLESTATUS:
      buffer << NDO_API_CONFIGVARIABLES << ":\n";
      handle_event<events::custom_variable_status>(
        *static_cast<events::custom_variable_status*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::DOWNTIME:
      buffer << NDO_API_DOWNTIMEDATA << ":\n";
      handle_event<events::downtime>(
        *static_cast<events::downtime*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::EVENTHANDLER:
      buffer << NDO_API_EVENTHANDLERDATA << ":\n";
      handle_event<events::event_handler>(
        *static_cast<events::event_handler*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::FLAPPINGSTATUS:
      buffer << NDO_API_FLAPPINGDATA << ":\n";
      handle_event<events::flapping_status>(
        *static_cast<events::flapping_status*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOST:
      buffer << NDO_API_HOSTDEFINITION << ":\n";
      handle_event<events::host>(
        *static_cast<events::host*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTCHECK:
      buffer << NDO_API_HOSTCHECKDATA << ":\n";
      handle_event<events::host_check>(
        *static_cast<events::host_check*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTDEPENDENCY:
      buffer << NDO_API_HOSTDEPENDENCYDEFINITION << ":\n";
      handle_event<events::host_dependency>(
        *static_cast<events::host_dependency*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTGROUP:
      buffer << NDO_API_HOSTGROUPDEFINITION << ":\n";
      handle_event<events::host_group>(
        *static_cast<events::host_group*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTGROUPMEMBER:
      buffer << NDO_API_HOSTGROUPMEMBERDEFINITION << ":\n";
      handle_event<events::host_group_member>(
        *static_cast<events::host_group_member*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTPARENT:
      buffer << NDO_API_HOSTPARENT << ":\n";
      handle_event<events::host_parent>(
        *static_cast<events::host_parent*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTSTATE:
      buffer << NDO_API_STATECHANGEDATA << ":\n";
      handle_event<events::host_state>(
        *static_cast<events::host_state*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::HOSTSTATUS:
      buffer << NDO_API_HOSTSTATUSDATA << ":\n";
      handle_event<events::host_status>(
        *static_cast<events::host_status*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::INSTANCE:
      buffer << NDO_API_PROCESSDATA << ":\n";
      handle_event<events::instance>(
        *static_cast<events::instance*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::INSTANCESTATUS:
      buffer << NDO_API_PROGRAMSTATUSDATA << ":\n";
      handle_event<events::instance_status>(
        *static_cast<events::instance_status*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::LOG:
      buffer << NDO_API_LOGDATA << ":\n";
      handle_event<events::log_entry>(
        *static_cast<events::log_entry*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::MODULE:
      buffer << NDO_API_COMMANDDEFINITION << ":\n";
      handle_event<events::module>(
        *static_cast<events::module*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::NOTIFICATION:
      buffer << NDO_API_NOTIFICATIONDATA << ":\n";
      handle_event<events::notification>(
        *static_cast<events::notification*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::SERVICE:
      buffer << NDO_API_SERVICEDEFINITION << ":\n";
      handle_event<events::service>(
        *static_cast<events::service*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::SERVICECHECK:
      buffer << NDO_API_SERVICECHECKDATA << ":\n";
      handle_event<events::service_check>(
        *static_cast<events::service_check*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::SERVICEDEPENDENCY:
      buffer << NDO_API_SERVICEDEPENDENCYDEFINITION << ":\n";
      handle_event<events::service_dependency>(
        *static_cast<events::service_dependency*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
     case events::event::SERVICEGROUP:
      buffer << NDO_API_SERVICEGROUPDEFINITION << ":\n";
      handle_event<events::service_group>(
        *static_cast<events::service_group*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::SERVICEGROUPMEMBER:
      buffer << NDO_API_SERVICEGROUPMEMBERDEFINITION << ":\n";
      handle_event<events::service_group_member>(
        *static_cast<events::service_group_member*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::SERVICESTATE:
      buffer << NDO_API_ADAPTIVESERVICEDATA << ":\n";
      handle_event<events::service_state>(
        *static_cast<events::service_state*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
     case events::event::SERVICESTATUS:
      buffer << NDO_API_SERVICESTATUSDATA << ":\n";
      handle_event<events::service_status>(
        *static_cast<events::service_status*>(e.data()),
        buffer);
      buffer << NDO_API_ENDDATA << "\n";
      break ;
    }
    buffer << "\n";

    // Send data.
    _to->write(buffer.str().c_str(), buffer.str().size());
  }
  catch (...) {}

  // Self event deregistration.
  e->remove_reader();

  return ;
}
