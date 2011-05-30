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

#include <assert.h>
#include <memory>
#include <stdlib.h>
#include <string.h>
#include "events/events.hh"
#include "interface/ndo/internal.hh"
#include "interface/ndo/source.hh"
#include "io/stream.hh"
#include "nagios/protoapi.h"

using namespace interface::ndo;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters from the data stream.
 */
template <typename T>
static T* handle_event(io::text& s) {
  std::auto_ptr<T> event(new T);
  int key;
  char const* key_str;
  char const* value_str;

  while (1) {
    key_str = s.line();
    if (key_str) {
      typename std::map<int, getter_setter<T> >::const_iterator it;
      key = strtol(key_str, NULL, 10);
      if (NDO_API_ENDDATA == key)
        break ;
      value_str = strchr(key_str, '=');
      value_str = (value_str ? value_str + 1 : "");
      it = ndo_mapped_type<T>::map.find(key);
      if (it != ndo_mapped_type<T>::map.end())
        (*it->second.setter)(*event.get(), *it->second.member, value_str);
    }
    else {
      event.reset();
      break ;
    }
  }
  return (event.release());
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  As source is not copyable, any attempt to use the copy constructor
 *  will result in a call to abort().
 *
 *  @param[in] s Unused.
 */
source::source(source const& s) : base(NULL), interface::source(s) {
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  As source is not copyable, any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  @param[in] s Unused.
 *
 *  @return This object.
 */
source& source::operator=(source const& s) {
  (void)s;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Constructor.
 *
 *  Build an NDO input source that uses the stream object as raw binary
 *  input. The stream object must not be NULL and is owned by the source
 *  object upon successful return from the constructor.
 *
 *  @param[in] s Input stream object.
 */
source::source(io::stream* s) : base(s) {}

/**
 *  Destructor.
 */
source::~source() {}

/**
 *  Close the source object.
 */
void source::close() {
  _stream.close();
  return ;
}

/**
 *  @brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the
 *  stream is closed.
 *
 *  @return Next available event, NULL if stream is closed.
 */
events::event* source::event() {
  std::auto_ptr<events::event> e;
  char const* line;

  // Get the next non-empty line.
  do {
    line = _stream.line();
  } while (line && !line[0]);

  if (line) {
    int id = strtol(line, NULL, 10);
    switch (id) {
     case NDO_API_ACKNOWLEDGEMENTDATA:
      e.reset(handle_event<events::acknowledgement>(_stream));
      break ;
     case NDO_API_COMMENTDATA:
      e.reset(handle_event<events::comment>(_stream));
      break ;
     case NDO_API_RUNTIMEVARIABLES:
      e.reset(handle_event<events::custom_variable>(_stream));
      break ;
     case NDO_API_CONFIGVARIABLES:
      e.reset(handle_event<events::custom_variable_status>(_stream));
      break ;
     case NDO_API_DOWNTIMEDATA:
      e.reset(handle_event<events::downtime>(_stream));
      break ;
     case NDO_API_EVENTHANDLERDATA:
      e.reset(handle_event<events::event_handler>(_stream));
      break ;
     case NDO_API_FLAPPINGDATA:
      e.reset(handle_event<events::flapping_status>(_stream));
      break ;
     case NDO_API_HOSTCHECKDATA:
      e.reset(handle_event<events::host_check>(_stream));
      break ;
     case NDO_API_HOSTDEFINITION:
      e.reset(handle_event<events::host>(_stream));
      break ;
     case NDO_API_HOSTDEPENDENCYDEFINITION:
      e.reset(handle_event<events::host_dependency>(_stream));
      break ;
     case NDO_API_HOSTGROUPDEFINITION:
      e.reset(handle_event<events::host_group>(_stream));
      break ;
     case NDO_API_HOSTGROUPMEMBERDEFINITION:
      e.reset(handle_event<events::host_group_member>(_stream));
      break ;
     case NDO_API_HOSTPARENT:
      e.reset(handle_event<events::host_parent>(_stream));
      break ;
     case NDO_API_STATECHANGEDATA:
      e.reset(handle_event<events::host_state>(_stream));
      break ;
     case NDO_API_HOSTSTATUSDATA:
      e.reset(handle_event<events::host_status>(_stream));
      break ;
     case NDO_API_PROCESSDATA:
      e.reset(handle_event<events::instance>(_stream));
      break ;
     case NDO_API_PROGRAMSTATUSDATA:
      e.reset(handle_event<events::instance_status>(_stream));
      break ;
     case NDO_API_LOGDATA:
      e.reset(handle_event<events::log_entry>(_stream));
      break ;
     case NDO_API_COMMANDDEFINITION:
      e.reset(handle_event<events::module>(_stream));
      break ;
     case NDO_API_NOTIFICATIONDATA:
      e.reset(handle_event<events::notification>(_stream));
      break ;
     case NDO_API_SERVICECHECKDATA:
      e.reset(handle_event<events::service_check>(_stream));
      break ;
     case NDO_API_SERVICEDEFINITION:
      e.reset(handle_event<events::service>(_stream));
      break ;
     case NDO_API_SERVICEDEPENDENCYDEFINITION:
      e.reset(handle_event<events::service_dependency>(_stream));
      break ;
     case NDO_API_SERVICEGROUPDEFINITION:
      e.reset(handle_event<events::service_group>(_stream));
      break ;
     case NDO_API_SERVICEGROUPMEMBERDEFINITION:
      e.reset(handle_event<events::service_group_member>(_stream));
      break ;
     case NDO_API_ADAPTIVESERVICEDATA:
      e.reset(handle_event<events::service_state>(_stream));
      break ;
     case NDO_API_SERVICESTATUSDATA:
      e.reset(handle_event<events::service_status>(_stream));
      break ;
     default:
      // Skip this event.
      while (id != NDO_API_ENDDATA) {
        line = _stream.line();
        if (line)
          id = strtol(line, NULL, 10);
        else
          break ;
      }
      if (line)
        return (this->event());
    }
  }
  if (e.get())
    e->add_reader();
  return (e.release());
}
