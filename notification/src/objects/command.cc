/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/command.hh"
#include "com/centreon/broker/notification/state.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Constructor from a base command string.
 *
 *  @param[in] base_command  The command from which to construct this object.
 */
command::command(std::string const& base_command) :
  _base_command(base_command) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
command::command(command const& obj) {
  command::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
command& command::operator=(command const& obj) {
  if (this != &obj) {
    _name = obj._name;
    _base_command = obj._base_command;
  }
  return (*this);
}

/**
 *  Get the name of this command.
 *
 *  @return  The name of this command.
 */
std::string const& command::get_name() const throw() {
  return (_name);
}

/**
 *  Set the name of this command.
 *
 *  @param[in] name  The new name of this command.
 */
void command::set_name(std::string const& name) {
  _name = name;
}

/*std::string command::resolve(neb::host_status) {
  std::string ret(_base_command);

  //string::replace_all(ret, "$HOSTADDRESS$", );
  return (ret);
}*/

/**
 *  Resolve this command.
 *
 *  @return  A string containing the resolved command.
 */
std::string command::resolve(
                       notification_rule::ptr const& rule,
                       timeperiod::ptr const& tp,
                       notification_method::ptr const& method,
                       contact::ptr const& contact,
                       node::ptr const& n,
                       node_cache const& cache) {
  return (std::string());
}


/**
 *  Replace a macro by its value.
 *
 *  @param[in] macro  The macro to replace.
 *  @param[in] state  The id of the node being resolved.
 *  @param[in] cache  A cache of data for the macros.
 *
 *  @return  The value of the macro.
 */
/*std::string command::_macro_replace(
                       std::string const& macro,
                       node_cache& cache) {

}*/
