/*
** Copyright 2014 Centreon
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

#include <ctime>
#include <sstream>
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/sql/cleanup.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_type           DB type.
 *  @param[in] db_host           DB server host.
 *  @param[in] db_port           DB server port.
 *  @param[in] db_user           DB user.
 *  @param[in] db_password       DB password.
 *  @param[in] db_name           DB name.
 *  @param[in] cleanup_interval  Cleanup interval.
 */
cleanup::cleanup(
           std::string const& db_type,
           std::string const& db_host,
           unsigned short db_port,
           std::string const& db_user,
           std::string const& db_password,
           std::string const& db_name,
           unsigned int cleanup_interval)
  : _db_type(db_type),
    _db_host(db_host),
    _db_port(db_port),
    _db_user(db_user),
    _db_password(db_password),
    _db_name(db_name),
    _interval(cleanup_interval),
    _should_exit(false) {}

/**
 *  Destructor.
 */
cleanup::~cleanup() throw () {}

/**
 *  Set the exit flag.
 */
void cleanup::exit() throw () {
  _should_exit = true;
  return ;
}

/**
 *  Get the rebuild check interval.
 *
 *  @return Rebuild check interval in seconds.
 */
unsigned int cleanup::get_interval() const throw () {
  return (_interval);
}

/**
 *  Thread entry point.
 */
void cleanup::run() {
  while (!_should_exit && _interval) {
    try {
      database db(database_config(
                   _db_type,
                   _db_host,
                   _db_port,
                   _db_user,
                   _db_password,
                   _db_name));
      database_query q(db);

      q.run_query(
        "UPDATE index_data"
        " INNER JOIN hosts ON index_data.host_id=hosts.host_id"
        " INNER JOIN instances ON hosts.instance_id=instances.instance_id"
        " SET index_data.to_delete=1"
        " WHERE instances.deleted=1",
        "could not flag the index_data table to delete outdated entries");
      q.run_query(
        "DELETE hosts FROM hosts INNER JOIN instances"
        " ON hosts.instance_id=instances.instance_id"
        " WHERE instances.deleted=1",
        "could not delete outdated entries from the hosts table");
      q.run_query(
        "DELETE modules FROM modules INNER JOIN instances"
        " ON modules.instance_id=instances.instance_id"
        " WHERE instances.deleted=1",
        "could not delete outdated entries from the modules table");
    }
    catch (std::exception const& e) {
      logging::error(logging::high) << "SQL: " << e.what();
    }
    catch (...) {
      logging::error(logging::high)
        << "SQL: unknown error occurred during cleanup";
    }

    // Sleep a while.
    time_t target(time(NULL) + _interval);
    while (!_should_exit && (target > time(NULL)))
      sleep(1);
  }
  return ;
}
