/*
** Copyright 2015 Merethis
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
 *  Check if command is addressed to some endpoint.
 *
 *  @param[in] endp_name  Endpoint name.
 *
 *  @return True if command request is addressed to specific endpoint.
 */
bool command_request::is_addressed_to(QString const& endp_name) const {
  return ((!destination_id || (destination_id == io::data::broker_id))
          && (endp == endp_name));
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
    &command_request::id,
    "id",
    mapping::entry::invalid_on_zero),
  mapping::entry()
};

// Operations.
static io::data* new_command_request() {
  return (new command_request);
}
io::event_info::event_operations const command_request::operations = {
  &new_command_request
};
