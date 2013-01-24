/*
** Copyright 2009-2013 Merethis
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

#include <algorithm>
#include <cstdlib>
#include <memory>
#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/ndo/input.hh"
#include "com/centreon/broker/ndo/internal.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/storage/events.hh"
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
 *  Read a line of input.
 */
char const* input::_get_line() {
  // Static type.
  static QString const io_raw_type("com::centreon::broker::io::raw");

  // Find new line.
  size_t it(_buffer.find_first_of('\n', _old));
  if (it == std::string::npos) {
    // Remove old data.
    _buffer.erase(0, _old);
    _old = 0;

    // Find a line.
    while ((it = _buffer.find_first_of('\n')) == std::string::npos) {
      misc::shared_ptr<io::data> data;
      _from->read(data);
      if (data.isNull())
        break ;
      if (data->type() == io_raw_type) {
        io::raw* raw(static_cast<io::raw*>(data.data()));
        _buffer.append(static_cast<char*>(
          raw->QByteArray::data()),
          raw->size());
      }
    }
  }

  // New base.
  size_t old(_old);

  // \n found.
  if (it != std::string::npos) {
    _buffer[it] = '\0';
    _old = it + 1;
  }
  // \n not found.
  else {
    _buffer.append('\0');
    _old = std::string::npos;
  }

  return (_buffer.c_str() + old);
}

/**
 *  Extract event parameters from the data stream.
 */
template <typename T>
T* input::_handle_event() {
  std::auto_ptr<T> event(new T);
  int key;
  char const* key_str;
  char const* value_str;

  while (1) {
    key_str = _get_line();
    if (key_str) {
      typename std::map<int, getter_setter<T> >::const_iterator it;
      key = strtol(key_str, NULL, 10);
      if (NDO_API_ENDDATA == key) {
        logging::debug(logging::medium)
          << "NDO: new event successfully generated";
        break ;
      }
      value_str = strchr(key_str, '=');
      value_str = (value_str ? value_str + 1 : "");
      it = ndo_mapped_type<T>::map.find(key);
      if (it != ndo_mapped_type<T>::map.end())
        (*it->second.setter)(
                       *event.get(),
                       *it->second.member,
                       value_str);
    }
    else {
      logging::debug(logging::medium)
        << "NDO: could not build a complete event";
      event.reset();
      break ;
    }
  }
  return (event.release());
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
input::input() : _old(0), _process_in(true) {}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
input::input(input const& i)
  : io::stream(i), _process_in(i._process_in) {}

/**
 *  Destructor.
 */
input::~input() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
input& input::operator=(input const& i) {
  if (this != &i) {
    io::stream::operator=(i);
    _process_in = i._process_in;
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
  // Return value.
  std::auto_ptr<io::data> e;
  d.clear();

  // Get the next non-empty line.
  logging::debug(logging::medium) << "NDO: reading event";
  char const* line;
  do {
    line = _get_line();
  } while (line && !line[0]);

  if (line) {
    int id = strtol(line, NULL, 10);
    switch (id) {
     case NDO_API_ACKNOWLEDGEMENTDATA:
      e.reset(_handle_event<neb::acknowledgement>());
      break ;
     case NDO_API_COMMENTDATA:
      e.reset(_handle_event<neb::comment>());
      break ;
     case NDO_API_RUNTIMEVARIABLES:
      e.reset(_handle_event<neb::custom_variable>());
      break ;
     case NDO_API_CONFIGVARIABLES:
      e.reset(_handle_event<neb::custom_variable_status>());
      break ;
     case NDO_API_DOWNTIMEDATA:
      e.reset(_handle_event<neb::downtime>());
      break ;
     case NDO_API_EVENTHANDLERDATA:
      e.reset(_handle_event<neb::event_handler>());
      break ;
     case NDO_API_FLAPPINGDATA:
      e.reset(_handle_event<neb::flapping_status>());
      break ;
     case NDO_API_HOSTCHECKDATA:
      e.reset(_handle_event<neb::host_check>());
      break ;
     case NDO_API_HOSTDEFINITION:
      e.reset(_handle_event<neb::host>());
      break ;
     case NDO_API_HOSTDEPENDENCYDEFINITION:
      e.reset(_handle_event<neb::host_dependency>());
      break ;
     case NDO_API_HOSTGROUPDEFINITION:
      e.reset(_handle_event<neb::host_group>());
      break ;
     case NDO_API_HOSTGROUPMEMBERDEFINITION:
      e.reset(_handle_event<neb::host_group_member>());
      break ;
     case NDO_API_HOSTPARENT:
      e.reset(_handle_event<neb::host_parent>());
      break ;
     case NDO_API_HOSTSTATUSDATA:
      e.reset(_handle_event<neb::host_status>());
      break ;
     case NDO_API_PROCESSDATA:
      e.reset(_handle_event<neb::instance>());
      break ;
     case NDO_API_PROGRAMSTATUSDATA:
      e.reset(_handle_event<neb::instance_status>());
      break ;
     case NDO_API_LOGDATA:
      e.reset(_handle_event<neb::log_entry>());
      break ;
     case NDO_API_COMMANDDEFINITION:
      e.reset(_handle_event<neb::module>());
      break ;
     case NDO_API_NOTIFICATIONDATA:
      e.reset(_handle_event<neb::notification>());
      break ;
     case NDO_API_SERVICECHECKDATA:
      e.reset(_handle_event<neb::service_check>());
      break ;
     case NDO_API_SERVICEDEFINITION:
      e.reset(_handle_event<neb::service>());
      break ;
     case NDO_API_SERVICEDEPENDENCYDEFINITION:
      e.reset(_handle_event<neb::service_dependency>());
      break ;
     case NDO_API_SERVICEGROUPDEFINITION:
      e.reset(_handle_event<neb::service_group>());
      break ;
     case NDO_API_SERVICEGROUPMEMBERDEFINITION:
      e.reset(_handle_event<neb::service_group_member>());
      break ;
     case NDO_API_SERVICESTATUSDATA:
      e.reset(_handle_event<neb::service_status>());
      break ;
     case NDO_API_STORAGEMETRIC:
      e.reset(_handle_event<storage::metric>());
      break ;
     case NDO_API_STORAGEREBUILD:
      e.reset(_handle_event<storage::rebuild>());
      break ;
     case NDO_API_STORAGEREMOVEGRAPH:
      e.reset(_handle_event<storage::remove_graph>());
      break ;
     case NDO_API_STORAGESTATUS:
      e.reset(_handle_event<storage::status>());
      break ;
     case NDO_API_CORRELATIONENGINESTATE:
      e.reset(_handle_event<correlation::engine_state>());
      break ;
     case NDO_API_CORRELATIONHOSTSTATE:
      e.reset(_handle_event<correlation::host_state>());
      break ;
     case NDO_API_CORRELATIONISSUE:
      e.reset(_handle_event<correlation::issue>());
      break ;
     case NDO_API_CORRELATIONISSUEPARENT:
      e.reset(_handle_event<correlation::issue_parent>());
      break ;
     case NDO_API_CORRELATIONSERVICESTATE:
      e.reset(_handle_event<correlation::service_state>());
      break ;
     default:
      // Skip this event.
      while (id != NDO_API_ENDDATA) {
        line = _get_line();
        if (line)
          id = strtol(line, NULL, 10);
        else
          break ;
      }
      if (line) {
        this->read(d);
        return ;
      }
    }
  }
  d = misc::shared_ptr<io::data>(e.get());
  e.release();
  return ;
}

/**
 *  Write data.
 *
 *  @param[in] d Object to copy.
 */
void input::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  throw (exceptions::msg() << "NDO: attempt to write to an input " \
           "object");
  return ;
}
