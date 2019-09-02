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
#include <QUuid>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/extcmd/command_request.hh"
#include "com/centreon/broker/extcmd/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::extcmd;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
command_request::command_request()
  : uuid(QUuid::createUuid().toString().toStdString()),
    with_partial_result(false) {}

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
 *  Check if command is addressed to some endpoint.
 *
 *  @param[in] endp_name  Endpoint name.
 *
 *  @return True if command request is addressed to specific endpoint.
 */
bool command_request::is_addressed_to(std::string const& endp_name) const {
  return (!destination_id || (destination_id == io::data::broker_id))
          && endp == endp_name;
}

/**
 *  @brief Parse a command string and store it in this object.
 *
 *  Command format is <BROKERID>;<TARGETENDPOINT>;<CMD>[;ARG1[;ARG2...]]
 *
 *  @param[in] cmdline  Command string.
 */
void command_request::parse(std::string const& cmdline) {
  // Get Broker ID.
  std::size_t delim1;
  delim1 = cmdline.find_first_of(';');
  if (delim1 == std::string::npos)
    throw (exceptions::msg() << "invalid command format: expected "
           << "<BROKERID>;<TARGETENDPOINT>;<CMD>[;<ARG1>[;<ARG2>...]]");
  destination_id = strtoul(cmdline.substr(0, delim1).c_str(), NULL, 0);

  // Get target endpoint.
  std::size_t delim2;
  delim2 = cmdline.find_first_of(';', delim1 + 1);
  if (delim2 == std::string::npos)
    throw (exceptions::msg() << "invalid command format: expected "
           << "<BROKERID>;<TARGETENDPOINT>;<CMD>[;<ARG1>[;<ARG2>...]]");
  endp = cmdline.substr(delim1 + 1, delim2 - delim1 - 1).c_str();

  // Get command.
  cmd = cmdline.substr(delim2 + 1).c_str();

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
  return (io::events::data_type<io::events::extcmd, io::events::de_command_request>::value);
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
  uuid = other.uuid;
  with_partial_result = other.with_partial_result;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const command_request::entries[] = {
  mapping::entry(
    &command_request::cmd,
    "cmd"),
  mapping::entry(
    &command_request::endp,
    "endp"),
  mapping::entry(
    &command_request::uuid,
    "uuid",
    mapping::entry::invalid_on_zero),
  mapping::entry(
    &command_request::with_partial_result,
    "with_partial_result"),
  mapping::entry()
};

// Operations.
static io::data* new_command_request() {
  return (new command_request);
}
io::event_info::event_operations const command_request::operations = {
  &new_command_request
};
