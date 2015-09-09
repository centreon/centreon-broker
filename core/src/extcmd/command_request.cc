/*
** Copyright 2015 Centreon
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

#include <cstdlib>
#include <QMutexLocker>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

unsigned int command_request::_id(0);
QMutex       command_request::_mutex;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
command_request::command_request() {
  QMutexLocker lock(&_mutex);
  id = ++_id;
}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
command_request::command_request(command_request const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
command_request::~command_request() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
command_request& command_request::operator=(
                                    command_request const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  @brief Parse a command string and store it in this object.
 *
 *  Command format is <BROKERID>;<TARGETENDPOINT>;<CMD>[;ARG1[;ARG2...]]
 *
 *  @param[in] cmdline  Command string.
 */
void command_request::parse(std::string const& cmdline) {
  // Get target endpoint.
  std::size_t delim;
  delim = cmdline.find_first_of(';');
  if (delim == std::string::npos)
    throw (exceptions::msg() << "invalid command format: expected "
           << "<TARGETENDPOINT>;<CMD>[;<ARG1>[;<ARG2>...]]");
  endp = cmdline.substr(0, delim).c_str();

  // Get command.
  cmd = cmdline.substr(delim + 1).c_str();

  return ;
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int command_request::type() const {
  return (command_request::static_type());
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int command_request::static_type() {
  return (io::events::data_type<io::events::internal, io::events::de_command_request>::value);
}


/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void command_request::_internal_copy(command_request const& other) {
  cmd = other.cmd;
  endp = other.endp;
  id = other.id;
  return ;
}
