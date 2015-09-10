/*
** Copyright 2012 Centreon
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
  ofs << "[" << time(NULL) << "] " << query << "\n";
  ofs.close();
  return ;
}

/**
 *  Execute a broker query on the command file.
 *
 *  @param[in] query  Query to execute.
 */
void external_command::execute_broker(std::string const& query) {
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
