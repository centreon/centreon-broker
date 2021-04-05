/*
** Copyright 2009-2021 Centreon
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

#include "com/centreon/broker/sql/stream.hh"

#include <ctime>
#include <limits>

#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/neb/downtime.hh"
#include "com/centreon/broker/neb/events.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/engine/common.hh"
#include "com/centreon/engine/host.hh"
#include "com/centreon/engine/service.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::sql;
using namespace com::centreon::broker::database;

/**
 *  Process log issue event.
 *
 *  @param[in] e  Uncasted log issue.
 */
void stream::_process_log_issue(std::shared_ptr<io::data> const& e) {
  // XXX : TODO
  (void)e;
}

/**
 *  Constructor.
 *
 *  @param[in] dbcfg                   Database configuration.
 *  @param[in] instance_timeout        Timeout of instances.
 *  @param[in] with_state_events       With state events.
 */
stream::stream(database_config const& dbcfg,
               uint32_t cleanup_check_interval,
               uint32_t loop_timeout,
               uint32_t instance_timeout,
               bool with_state_events)
    : io::stream("SQL"),
      _mysql(dbcfg),
      //      _cleanup_thread(dbcfg.get_type(),
      //                      dbcfg.get_host(),
      //                      dbcfg.get_port(),
      //                      dbcfg.get_user(),
      //                      dbcfg.get_password(),
      //                      dbcfg.get_name(),
      //                      cleanup_check_interval),
      _pending_events{0},
      _with_state_events(with_state_events),
      _stopped(false) {
  // FIXME DBR
  (void)cleanup_check_interval;
  //  // Get oudated instances.
  //
  //  // Run cleanup thread.
  //  _cleanup_thread.start();
  log_v2::sql()->debug("sql stream instanciation");
  if (!storage::conflict_manager::init_sql(dbcfg, loop_timeout,
                                           instance_timeout))
    throw msg_fmt(
        "SQL: Unable to initialize the sql connection to the database");
}

int32_t stream::stop() {
  // Stop cleanup thread.
  // _cleanup_thread.exit();
  int32_t retval = storage::conflict_manager::instance().unload(
      storage::conflict_manager::sql);
  _stopped = true;
  log_v2::core()->info("sql stream stopped with {} ackowledged events", retval);
  return retval;
}

/**
 *  Destructor.
 */
stream::~stream() {
  assert(_stopped);
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int stream::flush() {
  int32_t retval = storage::conflict_manager::instance().get_acks(
      storage::conflict_manager::sql);
  _pending_events -= retval;

  // Event acknowledgement.
  log_v2::sql()->debug("SQL: {} / {} events acknowledged", retval,
                       _pending_events);
  return retval;
}

/**
 *  Read from the database.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw exceptions::shutdown("cannot read from SQL database");
  return true;
}

/**
 *  Update internal stream cache.
 */
void stream::update() {}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;

  assert(data);

  // Process event.
  int32_t ack = storage::conflict_manager::instance().send_event(
      storage::conflict_manager::sql, data);
  _pending_events -= ack;
  return ack;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(json11::Json::object& tree) const {
  json11::Json::object obj{
      storage::conflict_manager::instance().get_statistics()};
  tree["sql pending events"] = _pending_events;
  tree["conflict_manager"] = obj;
}
