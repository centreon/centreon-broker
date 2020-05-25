/*
** Copyright 2009-2017 Centreon
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
#include <sstream>

#include "com/centreon/broker/correlation/events.hh"
#include "com/centreon/broker/correlation/internal.hh"
#include "com/centreon/broker/exceptions/msg.hh"
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

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::sql;
using namespace com::centreon::broker::database;

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Processing tables.
void (stream::*const stream::_correlation_processing_table[])(
    std::shared_ptr<io::data> const&) = {nullptr,
                                         &stream::_process_engine,
                                         &stream::_process_issue,
                                         &stream::_process_issue_parent,
                                         &stream::_process_state,
                                         &stream::_process_log_issue};

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Process a correlation engine event.
 *
 *  @param[in] e Uncasted correlation engine event.
 */
void stream::_process_engine(std::shared_ptr<io::data> const& e) {
  // Log message.
  logging::info(logging::medium) << "SQL: processing correlation engine event";

  // Cast event.
  correlation::engine_state const& es(
      *static_cast<correlation::engine_state const*>(e.get()));

  // Close issues.
  if (es.started) {
    time_t now(time(nullptr));
    {
      std::ostringstream ss;
      ss << "UPDATE issues SET end_time=" << now
         << "  WHERE end_time=0 OR end_time IS NULL";
      _mysql.run_query(ss.str());
    }
    {
      std::ostringstream ss;
      ss << "UPDATE issues_issues_parents SET end_time=" << now
         << "  WHERE end_time=0 OR end_time IS NULL";
      _mysql.run_query(ss.str());
    }
  }
}

/**
 *  Process a host state event.
 *
 *  @param[in] e Uncasted host state.
 */
void stream::_process_host_state(std::shared_ptr<io::data> const& e) {
  // Log message.
  correlation::state const& s(*static_cast<correlation::state const*>(e.get()));
  logging::info(logging::medium)
      << "SQL: processing host state event (host: " << s.host_id
      << ", state: " << s.current_state << ", start time: " << s.start_time
      << ", end time: " << s.end_time << ")";

  // Prepare queries.
  if (!_host_state_insupdate.prepared()) {
    std::ostringstream ss;
    ss << "INSERT INTO hoststateevents"
       << " (host_id, start_time, ack_time,"
          "            end_time, in_downtime, state)"
          "  VALUES (:host_id, :start_time, :ack_time, :end_time,"
          "          :in_downtime, :state)"
          " ON DUPLICATE KEY UPDATE"
          " ack_time=:ack_time,"
          " end_time=:end_time, in_downtime=:in_downtime,"
          " state=:state";
    _host_state_insupdate = mysql_stmt(ss.str(), true);
    _mysql.prepare_statement(_host_state_insupdate);
  }

  // Processing.
  if (_with_state_events) {
    std::ostringstream oss;
    oss << "SQL: could not store host state event (host: " << s.host_id
        << ", start time " << s.start_time << "): ";

    _host_state_insupdate << s;
    _mysql.run_statement(_host_state_insupdate, oss.str(), true,
                         _mysql.choose_connection_by_instance(s.poller_id));
  }
}

/**
 *  Process an issue event.
 *
 *  @param[in] e Uncasted issue.
 */
void stream::_process_issue(std::shared_ptr<io::data> const& e) {
  (void)e;
  //  // Issue object.
  //  correlation::issue const& i(*static_cast<correlation::issue
  //  const*>(e.get()));
  //
  //  // Log message.
  //  logging::info(logging::medium)
  //      << "SQL: processing issue event (node: (" << i.host_id << ", "
  //      << i.service_id << "), start time: " << i.start_time
  //      << ", end_time: " << i.end_time << ", ack time: " << i.ack_time <<
  //      ")";
  //
  //  // Prepare queries.
  //  if (!_issue_insupdate.prepared()) {
  //    query_preparator::event_unique unique;
  //    unique.insert("host_id");
  //    unique.insert("service_id");
  //    unique.insert("start_time");
  //    query_preparator qp(correlation::issue::static_type(), unique);
  //    _issue_insupdate = qp.prepare_insert_or_update(_mysql);
  //  }
  //
  //  // Processing.
  //  std::ostringstream oss;
  //  oss << "SQL: could not store issue (host: " << i.host_id
  //      << ", service: " << i.service_id << ", start time: " << i.start_time
  //      << "): ";
  //
  //  _issue_insupdate << i;
  //  _mysql.run_statement(
  //      _issue_insupdate, oss.str(), true,
  //      _mysql.choose_connection_by_instance(_cache_host_instance[i.host_id]));
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] e Uncasted issue parent.
 */
void stream::_process_issue_parent(std::shared_ptr<io::data> const& e) {
  (void)e;
  //  // Issue parent object.
  //  correlation::issue_parent const& ip(
  //      *static_cast<correlation::issue_parent const*>(e.get()));
  //
  //  // Log message.
  //  logging::info(logging::medium)
  //      << "SQL: processing issue parent "
  //         "event (child: ("
  //      << ip.child_host_id << ", " << ip.child_service_id << ", "
  //      << ip.child_start_time << "), parent: (" << ip.parent_host_id << ", "
  //      << ip.parent_service_id << ", " << ip.parent_start_time
  //      << "), start time: " << ip.start_time << ", end time: " << ip.end_time
  //      << ")";
  //
  //  // Database schema version.
  //  bool db_v2(_mysql.schema_version() == mysql::v2);
  //
  //  // Prepare queries.
  //  if (!_issue_parent_insert.prepared() || !_issue_parent_update.prepared())
  //  {
  //    {
  //      std::ostringstream ss;
  //      ss << "INSERT INTO "
  //         << (db_v2 ? "issues_issues_parents" : "rt_issues_issues_parents")
  //         << "  (child_id, end_time, start_time, parent_id)"
  //            "  VALUES (:child_id, :end_time, :start_time, :parent_id)";
  //      _issue_parent_insert = _mysql.prepare_query(ss.str());
  //    }
  //    {
  //      std::ostringstream ss;
  //      ss << "UPDATE "
  //         << (db_v2 ? "issues_issues_parents" : "rt_issues_issues_parents")
  //         << "  SET end_time=:end_time"
  //            "  WHERE child_id=:child_id"
  //            "    AND start_time=:start_time"
  //            "    AND parent_id=:parent_id";
  //      _issue_parent_update = _mysql.prepare_query(ss.str());
  //    }
  //  }
  //
  //  int child_id;
  //  int parent_id;
  //
  //  // Get child ID.
  //  {
  //    std::ostringstream query;
  //    query << "SELECT issue_id"
  //          << "  FROM " << (db_v2 ? "issues" : "rt_issues")
  //          << "  WHERE host_id=" << ip.child_host_id << " AND service_id";
  //    if (ip.child_service_id)
  //      query << "=" << ip.child_service_id;
  //    else
  //      query << " IS NULL";
  //    query << " AND start_time=" << ip.child_start_time;
  //
  //    std::promise<mysql_result> promise;
  //    _mysql.run_query_and_get_result(
  //        query.str(), &promise,
  //        _mysql.choose_connection_by_instance(
  //            _cache_host_instance[ip.child_host_id]));
  //    try {
  //      mysql_result res(promise.get_future().get());
  //      if (!_mysql.fetch_row(res))
  //        throw exceptions::msg() << "child issue does not exist";
  //
  //      child_id = res.value_as_i32(0);
  //      logging::debug(logging::low)
  //          << "SQL: child issue ID of (" << ip.child_host_id << ", "
  //          << ip.child_service_id << ", " << ip.child_start_time << ") is "
  //          << child_id;
  //    } catch (std::exception const& e) {
  //      throw exceptions::msg()
  //          << "SQL: could not fetch child issue "
  //             "ID (host: "
  //          << ip.child_host_id << ", service: " << ip.child_service_id
  //          << ", start: " << ip.child_start_time << "): " << e.what();
  //    }
  //  }
  //
  //  // Get parent ID.
  //  {
  //    std::ostringstream query;
  //    query << "SELECT issue_id"
  //             "  FROM "
  //          << (db_v2 ? "issues" : "rt_issues")
  //          << "  WHERE host_id=" << ip.parent_host_id << "    AND
  //          service_id";
  //    if (ip.parent_service_id)
  //      query << "=" << ip.parent_service_id;
  //    else
  //      query << " IS NULL";
  //    query << " AND start_time=" << ip.parent_start_time;
  //
  //    std::promise<mysql_result> promise;
  //    _mysql.run_query_and_get_result(
  //        query.str(), &promise,
  //        _mysql.choose_connection_by_instance(
  //            _cache_host_instance[ip.parent_host_id]));
  //    try {
  //      mysql_result res(promise.get_future().get());
  //      if (!_mysql.fetch_row(res))
  //        throw(exceptions::msg() << "parent issue does not exist");
  //
  //      parent_id = res.value_as_i32(0);
  //      logging::debug(logging::low)
  //          << "SQL: parent issue ID of (" << ip.parent_host_id << ", "
  //          << ip.parent_service_id << ", " << ip.parent_start_time << ") is "
  //          << parent_id;
  //    } catch (std::exception const& e) {
  //      throw exceptions::msg()
  //          << "SQL: could not fetch parent issue "
  //             "ID (host: "
  //          << ip.parent_host_id << ", service: " << ip.parent_service_id
  //          << ", start: " << ip.parent_start_time << "): " << e.what();
  //    }
  //  }
  //
  //  // End of parenting.
  //  if (ip.end_time != (time_t)-1) {
  //    _issue_parent_update.bind_value_as_u32(":end_time",
  //                                           static_cast<long
  //                                           long>(ip.end_time));
  //  } else
  //    _issue_parent_update.bind_value_as_null(":end_time");
  //  _issue_parent_update.bind_value_as_i32(":child_id", child_id);
  //  _issue_parent_update.bind_value_as_u32(":start_time",
  //                                         static_cast<long
  //                                         long>(ip.start_time));
  //  _issue_parent_update.bind_value_as_i32(":parent_id", parent_id);
  //  logging::debug(logging::low)
  //      << "SQL: updating issue parenting between child " << child_id
  //      << " and parent " << parent_id << " (start: " << ip.start_time
  //      << ", end: " << ip.end_time << ")";
  //  std::promise<int> promise;
  //  _mysql.run_statement_and_get_int(_issue_parent_update, &promise,
  //                                   mysql_task::AFFECTED_ROWS);
  //  try {
  //    if (promise.get_future().get() <= 0) {
  //      if (ip.end_time != (time_t)-1)
  //        _issue_parent_insert.bind_value_as_u32(
  //            ":end_time", static_cast<long long>(ip.end_time));
  //      else
  //        _issue_parent_insert.bind_value_as_null(":end_time");
  //      logging::debug(logging::low)
  //          << "SQL: inserting issue parenting between child " << child_id
  //          << " and parent " << parent_id << " (start: " << ip.start_time
  //          << ", end: " << ip.end_time << ")";
  //      _mysql.run_statement(_issue_parent_insert,
  //                           "SQL: issue parent insert query failed");
  //    }
  //  } catch (std::exception const& e) {
  //    throw exceptions::msg()
  //        << "SQL: issue parent update query failed: " << e.what();
  //  }
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_notification(std::shared_ptr<io::data> const& e
                                   __attribute__((unused))) {
  // XXX
  // // Log message.
  // logging::info(logging::medium)
  //   << "SQL: processing notification event";

  // // Processing.
  // _update_on_none_insert(
  //   _notification_insert,
  //   _notification_update,
  //   *static_cast<neb::notification const*>(e.data()));
}

/**
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(std::shared_ptr<io::data> const& e
                                    __attribute__((unused))) {
  //  // Log message.
  //  correlation::state const& s(*static_cast<correlation::state
  //  const*>(e.get())); logging::info(logging::medium)
  //      << "SQL: processing service state event (host: " << s.host_id
  //      << ", service: " << s.service_id << ", state: " << s.current_state
  //      << ", start time: " << s.start_time << ", end time: " << s.end_time
  //      << ")";
  //
  //  // Processing.
  //  if (_with_state_events) {
  //    // Prepare queries.
  //    if (!_service_state_insupdate.prepared()) {
  //      query_preparator::event_unique unique;
  //      unique.insert("host_id");
  //      unique.insert("service_id");
  //      unique.insert("start_time");
  //      query_preparator qp(correlation::state::static_type(), unique);
  //      _service_state_insupdate = qp.prepare_insert_or_update(_mysql);
  //    }
  //
  //    // Process object.
  //    std::ostringstream oss;
  //    oss << "SQL: could not store service state event (host: " << s.host_id
  //        << ", service: " << s.service_id << ", start time: " << s.start_time
  //        << "): ";
  //
  //    _service_state_insupdate << s;
  //    _mysql.run_statement(
  //        _service_state_insupdate, oss.str(), true,
  //        _mysql.choose_connection_by_instance(_cache_host_instance[s.host_id]));
  //  }
}

/**
 *  Process a state event.
 *
 *  @param[in] e  Uncasted state.
 */
void stream::_process_state(std::shared_ptr<io::data> const& e) {
  if (std::static_pointer_cast<correlation::state const>(e)->service_id == 0)
    _process_host_state(e);
  else
    _process_service_state(e);
}

/**
 *  Process log issue event.
 *
 *  @param[in] e  Uncasted log issue.
 */
void stream::_process_log_issue(std::shared_ptr<io::data> const& e) {
  // XXX : TODO
  (void)e;
}

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

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
    : _mysql(dbcfg),
      //      _cleanup_thread(dbcfg.get_type(),
      //                      dbcfg.get_host(),
      //                      dbcfg.get_port(),
      //                      dbcfg.get_user(),
      //                      dbcfg.get_password(),
      //                      dbcfg.get_name(),
      //                      cleanup_check_interval),
      _pending_events{0},
      _with_state_events(with_state_events) {
  // FIXME DBR
  (void)cleanup_check_interval;
  //  // Get oudated instances.
  //
  //  // Run cleanup thread.
  //  _cleanup_thread.start();
  log_v2::sql()->debug("sql stream instanciation");
  storage::conflict_manager::init_sql(dbcfg, loop_timeout, instance_timeout);
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Stop cleanup thread.
  //_cleanup_thread.exit();
  log_v2::sql()->debug("sql: stream destruction");
  storage::conflict_manager::unload();
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
  log_v2::sql()->debug("SQL: {} events have not yet been acknowledged",
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
  throw exceptions::shutdown() << "cannot read from SQL database";
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
//  if (!validate(data, "SQL"))
//    return 0;

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
