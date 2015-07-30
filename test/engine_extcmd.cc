/*
** Copyright 2012,2015 Merethis
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
