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
#include "test/engine_extcmd.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
engine_extcmd::engine_extcmd() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
engine_extcmd::engine_extcmd(engine_extcmd const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
engine_extcmd::~engine_extcmd() {
  ::remove(_file.c_str());
}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
engine_extcmd& engine_extcmd::operator=(engine_extcmd const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Execute a query on the command file.
 *
 *  @param[in] query Query to execute.
 */
void engine_extcmd::execute(std::string const& query) {
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
 *  Get the monitoring engine configuration.
 *
 *  @return Configuration entries.
 */
std::string engine_extcmd::get_engine_config() const {
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
std::string const& engine_extcmd::get_file() const throw () {
  return (_file);
}

/**
 *  Set the path to the external command file.
 *
 *  @param[in] file External command file path.
 */
void engine_extcmd::set_file(std::string const& file) {
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
 *  @param[in] other  Object to copy.
 */
void engine_extcmd::_internal_copy(engine_extcmd const& other) {
  _file = other._file;
  return ;
}
