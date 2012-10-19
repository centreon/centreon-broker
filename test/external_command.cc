/*
** Copyright 2012 Merethis
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

#include <cstdio>
#include <fstream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/external_command.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
external_command::external_command() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
external_command::external_command(external_command const& right) {
  _internal_copy(right);
}

/**
 *  Destructor.
 */
external_command::~external_command() {
  ::remove(_file.c_str());
}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
external_command& external_command::operator=(
                                      external_command const& right) {
  if (this != &right)
    _internal_copy(right);
  return (*this);
}

/**
 *  Execute a query on the command file.
 *
 *  @param[in] query Query to execute.
 */
void external_command::execute(std::string const& query) {
  std::ofstream ofs;
  ofs.open(_file.c_str(), std::ios_base::out | std::ios_base::app);
  if (ofs.fail())
    throw (exceptions::msg() << "cannot open command file '"
           << _file.c_str() << "'");
  ofs << query << "\n";
  ofs.close();
  return ;
}

/**
 *  Get the monitoring engine configuration.
 *
 *  @return Configuration entries.
 */
std::string external_command::get_engine_config() const {
  std::ostringstream config;
  config << "command_file=" << _file << "\n"
         << "command_check_interval=-1\n";
  return (config.str());
}

/**
 *  Get the path to the external command file.
 *
 *  @return External command file path.
 */
std::string const& external_command::get_file() const throw () {
  return (_file);
}

/**
 *  Set the path to the external command file.
 *
 *  @param[in] file External command file path.
 */
void external_command::set_file(std::string const& file) {
  _file = file;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] right Object to copy.
 */
void external_command::_internal_copy(external_command const& right) {
  _file = right._file;
  return ;
}
