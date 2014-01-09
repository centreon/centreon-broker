/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <cstdlib>
#include <memory>
#include <sstream>
#include "events/events.hh"
#include "interface/xml/destination.hh"
#include "interface/xml/internal.hh"
#include "io/stream.hh"

using namespace interface::xml;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters and send them to the data stream.
 */
template <typename T>
static void handle_event(T const& t,
                         umap<std::string, std::string>& attr) {
  for (typename umap<std::string, getter_setter<T> >::const_iterator
         it = xml_mapped_type<T>::map.begin(),
         end = xml_mapped_type<T>::map.end();
       it != end;
       ++it)
    (it->second.getter)(t, it->first, *it->second.member, attr);
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] s Stream on which data will be sent.
 */
destination::destination(io::stream* s) : _stream(s) {}

/**
 *  Destructor.
 */
destination::~destination() {}

/**
 *  Close the underlying stream.
 */
void destination::close() {
  _stream->close();
  return ;
}

/**
 *  Dump event to the stream.
 *
 *  @param[in] e Event to dump.
 */
void destination::event(events::event* e) {
  try {
    umap<std::string, std::string> attr;
    std::string name;
    switch (e->get_type()) {
     case events::event::ACKNOWLEDGEMENT:
      name = "acknowledgement";
      handle_event(*static_cast<events::acknowledgement*>(e), attr);
      break ;
     case events::event::COMMENT:
      name = "comment";
      handle_event(*static_cast<events::comment*>(e), attr);
      break ;
     case events::event::CUSTOMVARIABLE:
      name = "custom_variable";
      handle_event(*static_cast<events::custom_variable*>(e), attr);
      break ;
     case events::event::CUSTOMVARIABLESTATUS:
      name = "custom_variable_status";
      handle_event(*static_cast<events::custom_variable_status*>(e),
        attr);
      break ;
     case events::event::DOWNTIME:
      name = "downtime";
      handle_event(*static_cast<events::downtime*>(e), attr);
      break ;
     case events::event::EVENTHANDLER:
      name = "event_handler";
      handle_event(*static_cast<events::event_handler*>(e), attr);
      break ;
     case events::event::HOST:
      name = "host";
      handle_event(*static_cast<events::host*>(e), attr);
      break ;
     case events::event::HOSTDEPENDENCY:
      name = "host_dependency";
      handle_event(*static_cast<events::host_dependency*>(e), attr);
      break ;
     case events::event::HOSTGROUP:
      name = "host_group";
      handle_event(*static_cast<events::host_group*>(e), attr);
      break ;
     case events::event::HOSTGROUPMEMBER:
      name = "host_group_member";
      handle_event(*static_cast<events::host_group_member*>(e), attr);
      break ;
     case events::event::HOSTPARENT:
      name = "host_parent";
      handle_event(*static_cast<events::host_parent*>(e), attr);
      break ;
     case events::event::HOSTSTATE:
      name = "host_state";
      handle_event(*static_cast<events::host_state*>(e), attr);
      break ;
     case events::event::HOSTSTATUS:
      name = "host_status";
      handle_event(*static_cast<events::host_status*>(e), attr);
      break ;
     case events::event::INSTANCE:
      name = "instance";
      handle_event(*static_cast<events::instance*>(e), attr);
      break ;
     case events::event::INSTANCESTATUS:
      name = "instance_status";
      handle_event(*static_cast<events::instance_status*>(e), attr);
      break ;
     case events::event::LOG:
      name = "log_entry";
      handle_event(*static_cast<events::log_entry*>(e), attr);
      break ;
     case events::event::MODULE:
      name = "module";
      handle_event(*static_cast<events::module*>(e), attr);
      break ;
     case events::event::NOTIFICATION:
      name = "notification";
      handle_event(*static_cast<events::notification*>(e), attr);
      break ;
     case events::event::SERVICE:
      name = "service";
      handle_event(*static_cast<events::service*>(e), attr);
      break ;
     case events::event::SERVICEDEPENDENCY:
      name = "service_dependency";
      handle_event(*static_cast<events::service_dependency*>(e), attr);
      break ;
     case events::event::SERVICEGROUP:
      name = "service_group";
      handle_event(*static_cast<events::service_group*>(e), attr);
      break ;
     case events::event::SERVICEGROUPMEMBER:
      name = "service_group_member";
      handle_event(*static_cast<events::service_group_member*>(e),
        attr);
      break ;
     case events::event::SERVICESTATE:
      name = "service_state";
      handle_event(*static_cast<events::service_state*>(e), attr);
      break ;
     case events::event::SERVICESTATUS:
      name = "service_status";
      handle_event(*static_cast<events::service_status*>(e), attr);
      break ;
     default:
      return ;
    }

    // Send XML node.
    std::string data;
    data = "<";
    data.append(name);
    for (umap<std::string, std::string>::const_iterator
           it = attr.begin(),
           end = attr.end();
         it != end;
         ++it) {
      data.append(" ");
      data.append(it->first);
      data.append("=\"");
      data.append(it->second);
      data.append("\"");
    }
    data.append(" />\n");
    _stream->send(data.c_str(), data.size());
  }
  catch (...) {}

  // Self event deregistration.
  e->remove_reader();

  return ;
}
