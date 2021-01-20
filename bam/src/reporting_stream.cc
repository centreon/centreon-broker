/*
** Copyright 2014-2015,2017 Centreon
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

#include "com/centreon/broker/bam/reporting_stream.hh"

#include <cstdlib>

#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exclusion.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/database/table_max_size.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/time/timezone_manager.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;
using namespace com::centreon::broker::bam;
using namespace com::centreon::broker::database;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg                  BAM DB configuration.
 */
reporting_stream::reporting_stream(database_config const& db_cfg)
    : io::stream("BAM-BI"),
      _ack_events(0),
      _pending_events(0),
      _mysql(db_cfg),
      _processing_dimensions(false) {
  // Prepare queries.
  _prepare();

  // Load timeperiods.
  _load_timeperiods();

  // Close inconsistent events.
  _close_inconsistent_events("BA", "mod_bam_reporting_ba_events", "ba_id");
  _close_inconsistent_events("KPI", "mod_bam_reporting_kpi_events", "kpi_id");

  // Close remaining events.
  _close_all_events();

  // Initialize the availabilities thread.
  _availabilities.reset(new availability_thread(db_cfg, _timeperiods));
  _availabilities->start_and_wait();
}

/**
 *  Destructor.
 */
reporting_stream::~reporting_stream() {
  // Terminate the availabilities thread.
  _availabilities->terminate();
  _availabilities->wait();
  log_v2::sql()->debug("bam: reporting_stream destruction");
}

/**
 *  Read from the database.
 *  Get the next available bam event.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool reporting_stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw exceptions::shutdown("cannot read from BAM reporting stream");
  return true;
}
/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void reporting_stream::statistics(json11::Json::object& tree) const {
  std::lock_guard<std::mutex> lock(_statusm);
  if (!_status.empty())
    tree["status"] = _status;
}

/**
 *  Flush the stream.
 *
 *  @return Number of acknowledged events.
 */
int reporting_stream::flush() {
  _mysql.commit();
  int retval(_ack_events + _pending_events);
  _ack_events = 0;
  _pending_events = 0;
  return retval;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int reporting_stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;
  if (!validate(data, "BAM-BI"))
    return (0);

  switch (data->type()) {
    case io::events::data_type<io::events::bam, bam::de_kpi_event>::value:
      _process_kpi_event(data);
      break;
    case io::events::data_type<io::events::bam, bam::de_ba_event>::value:
      _process_ba_event(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_ba_duration_event>::value:
      _process_ba_duration_event(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_truncate_table_signal>::value:
      _process_dimension_truncate_signal(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_ba_event>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_bv_event>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_ba_bv_relation_event>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_kpi_event>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_timeperiod>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_timeperiod_exception>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_timeperiod_exclusion>::value:
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_ba_timeperiod_relation>::value:
      _process_dimension(data);
      break;
    case io::events::data_type<io::events::bam, bam::de_rebuild>::value:
      _process_rebuild(data);
      break;
    default:
      break;
  }

  // Event acknowledgement.
  int retval(_ack_events);
  _ack_events = 0;
  return retval;
}

/**
 *  Apply a timeperiod declaration.
 *
 *  @param[in] tp  Timeperiod declaration.
 */
void reporting_stream::_apply(dimension_timeperiod const& tp) {
  log_v2::bam()->trace("BAM-BI: adding timeperiod {} to cache", tp.id);
  _timeperiods.add_timeperiod(
      tp.id, time::timeperiod::ptr(new time::timeperiod(
                 tp.id, tp.name, "", tp.sunday, tp.monday, tp.tuesday,
                 tp.wednesday, tp.thursday, tp.friday, tp.saturday)));
}

/**
 *  Apply a timeperiod exception declaration.
 *
 *  @param[in] tpe  Timeperiod exclusion declaration.
 */
void reporting_stream::_apply(dimension_timeperiod_exception const& tpe) {
  time::timeperiod::ptr timeperiod =
      _timeperiods.get_timeperiod(tpe.timeperiod_id);
  if (timeperiod)
    timeperiod->add_exception(tpe.daterange, tpe.timerange);
  else
    logging::error(logging::medium)
        << "BAM-BI: could not apply exception on timeperiod "
        << tpe.timeperiod_id << ": timeperiod does not exist";
}

/**
 *  Apply a timeperiod exclusion declaration.
 *
 *  @param[in] tpe  Timeperiod exclusion declaration.
 */
void reporting_stream::_apply(dimension_timeperiod_exclusion const& tpe) {
  time::timeperiod::ptr timeperiod =
      _timeperiods.get_timeperiod(tpe.timeperiod_id);
  time::timeperiod::ptr excluded_tp =
      _timeperiods.get_timeperiod(tpe.excluded_timeperiod_id);
  if (timeperiod && excluded_tp)
    timeperiod->add_excluded(excluded_tp);
  else
    logging::error(logging::medium)
        << "BAM-BI: could not apply exclusion of timeperiod "
        << tpe.excluded_timeperiod_id << " by timeperiod " << tpe.timeperiod_id
        << ": at least one of the timeperiod does not exist";
}

/**
 *  Delete inconsistent events.
 *
 *  @param[in] event_type  Event type (BA or KPI).
 *  @param[in] table       Table name.
 *  @param[in] id          Table ID name.
 */
void reporting_stream::_close_inconsistent_events(char const* event_type,
                                                  char const* table,
                                                  char const* id) {
  // Get events to close.
  std::list<std::pair<uint32_t, time_t>> events;
  {
    std::string query(
        fmt::format("SELECT e1.{0}, e1.start_time FROM {1} AS e1 INNER JOIN "
                    "(SELECT {0}, MAX(start_time) AS max_start_time FROM {1} "
                    "GROUP BY {0}) AS e2 ON e1.{0}=e2.{0} WHERE e1.end_time IS "
                    "NULL AND e1.start_time!=e2.max_start_time",
                    id, table));
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        events.emplace_back(std::make_pair(
            res.value_as_u32(0), static_cast<time_t>(res.value_as_i32(1))));
    } catch (std::exception const& e) {
      throw msg_fmt("BAM-BI: could not get inconsistent events: {}", e.what());
    }
  }

  // Close each event.
  for (std::list<std::pair<uint32_t, time_t>>::const_iterator
           it(events.begin()),
       end(events.end());
       it != end; ++it) {
    time_t end_time;
    {
      std::string query_str(
          fmt::format("SELECT start_time FROM {} WHERE {}={} AND start_time>{} "
                      "ORDER BY start_time ASC LIMIT 1",
                      table, id, it->first, it->second));
      std::promise<mysql_result> promise;
      _mysql.run_query_and_get_result(query_str, &promise);
      try {
        mysql_result res(promise.get_future().get());
        if (!_mysql.fetch_row(res))
          throw msg_fmt("no event following this one");

        end_time = res.value_as_i32(0);
      } catch (std::exception const& e) {
        throw msg_fmt(
            "BAM-BI: could not get end time of inconsistent event of {} {} "
            "starting"
            " at {} : {}",
            event_type, it->first, it->second, e.what());
      }
    }
    {
      std::string query(
          fmt::format("UPDATE {} SET end_time={} WHERE {}={} AND start_time={}",
                      table, end_time, id, it->first, it->second));
      _mysql.run_query(query, database::mysql_error::close_event, true);
    }
  }
}

void reporting_stream::_close_all_events() {
  time_t now(::time(nullptr));
  std::string query(
      fmt::format("UPDATE mod_bam_reporting_ba_events SET end_time={} WHERE "
                  "end_time IS NULL",
                  now));
  _mysql.run_query(query, database::mysql_error::close_ba_events);

  query = fmt::format(
      "UPDATE mod_bam_reporting_kpi_events SET end_time={} WHERE end_time IS "
      "NULL",
      now);
  _mysql.run_query(query, database::mysql_error::close_kpi_events);
}

/**
 *  Load timeperiods from DB.
 */
void reporting_stream::_load_timeperiods() {
  // Clear old timeperiods.
  _timeperiods.clear();

  // Load timeperiods.
  {
    std::string query(
        "SELECT timeperiod_id, name, sunday, monday, tuesday, wednesday, "
        "thursday, friday, saturday FROM mod_bam_reporting_timeperiods");
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        _timeperiods.add_timeperiod(
            res.value_as_u32(0),
            time::timeperiod::ptr(new time::timeperiod(
                res.value_as_u32(0), res.value_as_str(1), "",
                res.value_as_str(2), res.value_as_str(3), res.value_as_str(4),
                res.value_as_str(5), res.value_as_str(6), res.value_as_str(7),
                res.value_as_str(8))));
      }
    } catch (std::exception const& e) {
      throw msg_fmt("BAM-BI: could not load timeperiods from DB: {}", e.what());
    }
  }

  // Load exceptions.
  {
    std::string query(
        "SELECT timeperiod_id, daterange, timerange FROM "
        "mod_bam_reporting_timeperiods_exceptions");
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        time::timeperiod::ptr tp =
            _timeperiods.get_timeperiod(res.value_as_u32(0));
        if (!tp)
          logging::error(logging::high)
              << "BAM-BI: could not apply exception to non-existing timeperiod "
              << res.value_as_u32(0);
        else
          tp->add_exception(res.value_as_str(1), res.value_as_str(2));
      }
    } catch (std::exception const& e) {
      throw msg_fmt("BAM-BI: could not load timeperiods exceptions from DB: {}",
                    e.what());
    }
  }

  // Load exclusions.
  {
    std::string query(
        "SELECT timeperiod_id, excluded_timeperiod_id"
        "  FROM mod_bam_reporting_timeperiods_exclusions");
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res)) {
        time::timeperiod::ptr tp =
            _timeperiods.get_timeperiod(res.value_as_u32(0));
        time::timeperiod::ptr excluded_tp =
            _timeperiods.get_timeperiod(res.value_as_u32(1));
        if (!tp || !excluded_tp)
          logging::error(logging::high)
              << "BAM-BI: could not apply exclusion of timeperiod "
              << res.value_as_u32(1) << " by timeperiod " << res.value_as_u32(0)
              << ": at least one timeperiod does not exist";
        else
          tp->add_excluded(excluded_tp);
      }
    } catch (std::exception const& e) {
      throw msg_fmt("BAM-BI: could not load exclusions from DB: {} ", e.what());
    }
  }

  // Load BA/timeperiods relations.
  {
    std::string query(
        "SELECT ba_id, timeperiod_id, is_default"
        "  FROM mod_bam_reporting_relations_ba_timeperiods");
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query, &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        _timeperiods.add_relation(res.value_as_u32(0), res.value_as_u32(1),
                                  res.value_as_bool(2));
    } catch (std::exception const& e) {
      throw msg_fmt("BAM-BI: could not load BA/timeperiods relations: {}",
                    e.what());
    }
  }
}

/**
 *  Prepare queries.
 */
void reporting_stream::_prepare() {
  std::string query;

  query =
      "INSERT INTO mod_bam_reporting_ba_events (ba_id,"
      "first_level,start_time,end_time,status,in_downtime)"
      " VALUES(?,?,?,?,?,?)";
  _ba_full_event_insert = _mysql.prepare_query(query);

  query =
      "UPDATE mod_bam_reporting_ba_events"
      "  SET end_time=?, first_level=?,"
      "      status=?, in_downtime=?"
      "  WHERE ba_id=? AND start_time=?";
  _ba_event_update = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_ba_events_durations ("
      "                ba_event_id, start_time, "
      "                end_time, duration, sla_duration, timeperiod_id, "
      "                timeperiod_is_default)"
      "  SELECT b.ba_event_id, ?, ?, ?, "
      "         ?, ?, ?"
      "  FROM mod_bam_reporting_ba_events AS b"
      "  WHERE b.ba_id=? AND b.start_time=?";
  _ba_duration_event_insert = _mysql.prepare_query(query);

  query =
      "UPDATE mod_bam_reporting_ba_events_durations AS d"
      "  INNER JOIN mod_bam_reporting_ba_events AS e"
      "    ON d.ba_event_id=e.ba_event_id"
      "  SET d.start_time=?, d.end_time=?, "
      "      d.duration=?, d.sla_duration=?,"
      "      d.timeperiod_is_default=?"
      "  WHERE e.ba_id=?"
      "    AND e.start_time=?"
      "    AND d.timeperiod_id=?";
  _ba_duration_event_update = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_kpi_events (kpi_id,"
      " start_time, end_time, status, in_downtime,"
      " impact_level) VALUES (?, ?, ?, ?, ?, ?)";
  _kpi_full_event_insert = _mysql.prepare_query(query);

  query =
      "UPDATE mod_bam_reporting_kpi_events"
      " SET end_time=?, status=?,"
      " in_downtime=?, impact_level=?"
      " WHERE kpi_id=? AND start_time=?";
  _kpi_event_update = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_relations_ba_kpi_events"
      " (ba_event_id, kpi_event_id)"
      " SELECT be.ba_event_id, ke.kpi_event_id"
      " FROM mod_bam_reporting_kpi_events AS ke"
      " LEFT JOIN mod_bam_reporting_ba_events AS be"
      " ON (ke.start_time >= be.start_time"
      " AND (be.end_time IS NULL OR ke.start_time < be.end_time))"
      " WHERE ke.kpi_id=? AND ke.start_time=? AND be.ba_id=?";
  _kpi_event_link = _mysql.prepare_query(query);

  query =
      "UPDATE mod_bam_reporting_relations_ba_kpi_events"
      " SET ba_event_id=?"
      " WHERE relation_id=?";
  _kpi_event_link_update = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
      "                sla_month_percent_crit, sla_month_percent_warn,"
      "                sla_month_duration_crit, sla_month_duration_warn)"
      " VALUES (?, ?, ?, ?,"
      "         ?, ?,"
      "         ?)";
  _dimension_ba_insert = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_bv (bv_id, bv_name, bv_description)"
      "  VALUES (?, ?, ?)";
  _dimension_bv_insert = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_relations_ba_bv (ba_id, bv_id)"
      "  VALUES (?, ?)";
  _dimension_ba_bv_relation_insert = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_timeperiods"
      "            (timeperiod_id, name, sunday, monday,"
      "             tuesday, wednesday, thursday, friday,"
      "             saturday)"
      "  VALUES (?, ?, ?, ?,"
      "          ?, ?, ?, ?,"
      "          ?)";
  _dimension_timeperiod_insert = _mysql.prepare_query(query);

  query =
      "INSERT INTO mod_bam_reporting_timeperiods_exceptions"
      "            (timeperiod_id, daterange, timerange)"
      "  VALUES (?, ?, ?)";
  _dimension_timeperiod_exception_insert = _mysql.prepare_query(query);

  // Dimension timeperiod exclusion insertion.
  query =
      "INSERT INTO mod_bam_reporting_timeperiods_exclusions"
      "            (timeperiod_id, excluded_timeperiod_id)"
      "  VALUES (?, ?)";
  _dimension_timeperiod_exclusion_insert = _mysql.prepare_query(query);

  // Dimension BA/timeperiod insertion.
  query =
      "INSERT INTO mod_bam_reporting_relations_ba_timeperiods ("
      "            ba_id, timeperiod_id, is_default)"
      "  VALUES (?, ?, ?)";
  _dimension_ba_timeperiod_insert = _mysql.prepare_query(query);

  _dimension_truncate_tables.clear();
  query = "DELETE FROM mod_bam_reporting_kpi";
  _dimension_truncate_tables.push_back(_mysql.prepare_query(query));
  query = "DELETE FROM mod_bam_reporting_relations_ba_bv";
  _dimension_truncate_tables.push_back(_mysql.prepare_query(query));
  query = "DELETE FROM mod_bam_reporting_ba";
  _dimension_truncate_tables.push_back(_mysql.prepare_query(query));
  query = "DELETE FROM mod_bam_reporting_bv";
  _dimension_truncate_tables.push_back(_mysql.prepare_query(query));
  query = "DELETE FROM mod_bam_reporting_timeperiods";
  _dimension_truncate_tables.push_back(_mysql.prepare_query(query));

  // Dimension KPI insertion
  query =
      "INSERT INTO mod_bam_reporting_kpi (kpi_id, kpi_name,"
      "            ba_id, ba_name, host_id, host_name,"
      "            service_id, service_description, kpi_ba_id,"
      "            kpi_ba_name, meta_service_id, meta_service_name,"
      "            impact_warning, impact_critical, impact_unknown,"
      "            boolean_id, boolean_name)"
      "  VALUES (?, ?, ?, ?, ?,"
      "          ?, ?, ?,"
      "          ?, ?, ?,"
      "          ?, ?, ?,"
      "          ?, ?, ?)";
  _dimension_kpi_insert = _mysql.prepare_query(query);
}

/**
 *  Process a ba event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_ba_event(std::shared_ptr<io::data> const& e) {
  bam::ba_event const& be = *std::static_pointer_cast<bam::ba_event const>(e);
  log_v2::bam()->debug(
      "BAM-BI: processing event of BA {} (start time {}, end time {}, status "
      "{}, in downtime {})",
      be.ba_id, be.start_time, be.end_time, be.status, be.in_downtime);

  // Try to update event.
  if (be.end_time.is_null())
    _ba_event_update.bind_value_as_null(0);
  else
    _ba_event_update.bind_value_as_u64(0, be.end_time.get_time_t());
  _ba_event_update.bind_value_as_i32(1, be.first_level);
  _ba_event_update.bind_value_as_tiny(2, be.status);
  _ba_event_update.bind_value_as_bool(3, be.in_downtime);
  _ba_event_update.bind_value_as_i32(4, be.ba_id);
  _ba_event_update.bind_value_as_u64(
      5, static_cast<uint64_t>(be.start_time.get_time_t()));

  std::promise<int> promise;
  _mysql.run_statement_and_get_int<int>(_ba_event_update, &promise,
                                        mysql_task::int_type::AFFECTED_ROWS);

  // Event was not found, insert one.
  try {
    if (promise.get_future().get() == 0) {
      _ba_full_event_insert.bind_value_as_i32(0, be.ba_id);
      _ba_full_event_insert.bind_value_as_i32(1, be.first_level);
      _ba_full_event_insert.bind_value_as_u64(
          2, static_cast<uint64_t>(be.start_time.get_time_t()));

      if (be.end_time.is_null())
        _ba_full_event_insert.bind_value_as_null(3);
      else
        _ba_full_event_insert.bind_value_as_u64(
            3, static_cast<uint64_t>(be.end_time.get_time_t()));
      _ba_full_event_insert.bind_value_as_tiny(4, be.status);
      _ba_full_event_insert.bind_value_as_bool(5, be.in_downtime);

      std::promise<uint32_t> result;
      _mysql.run_statement_and_get_int<uint32_t>(
          _ba_full_event_insert, &result, mysql_task::LAST_INSERT_ID, -1);
      uint32_t newba = result.get_future().get();
      // check events for BA
      if (_last_inserted_kpi.find(be.ba_id) != _last_inserted_kpi.end()) {
        std::map<std::time_t, uint64_t>& m_events =
            _last_inserted_kpi[be.ba_id];
        if (m_events.find(be.start_time.get_time_t()) != m_events.end()) {
          // Insert kpi event link.
          _kpi_event_link_update.bind_value_as_i32(0, newba);
          _kpi_event_link_update.bind_value_as_u64(
              1, m_events[be.start_time.get_time_t()]);
          _mysql.run_statement(_kpi_event_link_update,
                               database::mysql_error::update_kpi_event, true);
        }
        // remove older events for BA
        for (auto it = m_events.begin(); it != m_events.end();) {
          if (it->first < be.start_time.get_time_t())
            it = m_events.erase(it);
          else
            break;
        }
      }
    }
  } catch (std::exception const& e) {
    throw msg_fmt(
        "BAM-BI: could not update event of BA {} "
        " starting at {} and ending at {}: {}",
        be.ba_id, be.start_time, be.end_time, e.what());
  }

  // Compute the associated event durations.
  if (!be.end_time.is_null() && be.start_time != be.end_time)
    _compute_event_durations(std::static_pointer_cast<bam::ba_event>(e), this);
}

/**
 *  Process a ba duration event and write it to the db.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_ba_duration_event(
    std::shared_ptr<io::data> const& e) {
  bam::ba_duration_event const& bde =
      *std::static_pointer_cast<bam::ba_duration_event const>(e);
  log_v2::bam()->debug(
      "BAM-BI: processing BA duration event of BA {} (start time {}, end time "
      "{}, duration {}, sla duration {})",
      bde.ba_id, bde.start_time, bde.end_time, bde.duration, bde.sla_duration);

  // Try to update first.
  _ba_duration_event_update.bind_value_as_u64(
      1, static_cast<uint64_t>(bde.end_time.get_time_t()));
  _ba_duration_event_update.bind_value_as_u64(
      0, static_cast<uint64_t>(bde.start_time.get_time_t()));
  _ba_duration_event_update.bind_value_as_i32(2, bde.duration);
  _ba_duration_event_update.bind_value_as_i32(3, bde.sla_duration);
  _ba_duration_event_update.bind_value_as_i32(4, bde.timeperiod_is_default);
  _ba_duration_event_update.bind_value_as_i32(5, bde.ba_id);
  _ba_duration_event_update.bind_value_as_u64(
      6, static_cast<uint64_t>(bde.real_start_time.get_time_t()));
  _ba_duration_event_update.bind_value_as_i32(7, bde.timeperiod_id);

  std::promise<int> promise;
  int thread_id(_mysql.run_statement_and_get_int<int>(
      _ba_duration_event_update, &promise,
      mysql_task::int_type::AFFECTED_ROWS));
  try {
    // Insert if no rows was updated.
    if (promise.get_future().get() == 0) {
      _ba_duration_event_insert.bind_value_as_u64(
          0, static_cast<uint64_t>(bde.start_time.get_time_t()));
      _ba_duration_event_insert.bind_value_as_u64(
          1, static_cast<uint64_t>(bde.end_time.get_time_t()));
      _ba_duration_event_insert.bind_value_as_i32(2, bde.duration);
      _ba_duration_event_insert.bind_value_as_i32(3, bde.sla_duration);
      _ba_duration_event_insert.bind_value_as_i32(4, bde.timeperiod_id);
      _ba_duration_event_insert.bind_value_as_f64(5, bde.timeperiod_is_default);
      _ba_duration_event_insert.bind_value_as_i32(6, bde.ba_id);
      _ba_duration_event_insert.bind_value_as_u64(
          7, static_cast<uint64_t>(bde.real_start_time.get_time_t()));

      _mysql.run_statement(_ba_duration_event_insert,
                           database::mysql_error::empty, true, thread_id);
    }
  } catch (std::exception const& e) {
    throw msg_fmt(
        "BAM-BI: could not insert duration event of BA {}"
        " starting at {} : {}",
        bde.ba_id, bde.start_time, e.what());
  }
}

/**
 *  Process a kpi event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_kpi_event(std::shared_ptr<io::data> const& e) {
  bam::kpi_event const& ke = *std::static_pointer_cast<bam::kpi_event const>(e);
  log_v2::bam()->debug(
      "BAM-BI: processing event of KPI {} (start time {}, end time {}, state "
      "{}, in downtime {})",
      ke.kpi_id, ke.start_time, ke.end_time, ke.status, ke.in_downtime);

  // Try to update kpi.
  if (ke.end_time.is_null())
    _kpi_event_update.bind_value_as_null(0);
  else
    _kpi_event_update.bind_value_as_u64(
        0, static_cast<uint64_t>(ke.end_time.get_time_t()));
  _kpi_event_update.bind_value_as_tiny(1, ke.status);
  _kpi_event_update.bind_value_as_i32(2, ke.in_downtime);
  _kpi_event_update.bind_value_as_i32(3, ke.impact_level);
  _kpi_event_update.bind_value_as_i32(4, ke.kpi_id);
  _kpi_event_update.bind_value_as_u64(
      5, static_cast<uint64_t>(ke.start_time.get_time_t()));

  std::promise<int> promise;
  int thread_id(_mysql.run_statement_and_get_int<int>(
      _kpi_event_update, &promise, mysql_task::int_type::AFFECTED_ROWS));
  // No kpis were updated, insert one.
  try {
    if (promise.get_future().get() == 0) {
      _kpi_full_event_insert.bind_value_as_i32(0, ke.kpi_id);
      _kpi_full_event_insert.bind_value_as_u64(
          1, static_cast<uint64_t>(ke.start_time.get_time_t()));
      if (ke.end_time.is_null())
        _kpi_full_event_insert.bind_value_as_null(2);
      else
        _kpi_full_event_insert.bind_value_as_u64(
            2, static_cast<uint64_t>(ke.end_time.get_time_t()));
      _kpi_full_event_insert.bind_value_as_tiny(3, ke.status);
      _kpi_full_event_insert.bind_value_as_bool(4, ke.in_downtime);
      _kpi_full_event_insert.bind_value_as_i32(5, ke.impact_level);

      _mysql.run_statement(_kpi_full_event_insert,
                           database::mysql_error::insert_kpi_event, true,
                           thread_id);

      // Insert kpi event link.
      _kpi_event_link.bind_value_as_i32(0, ke.kpi_id);
      _kpi_event_link.bind_value_as_u64(
          1, static_cast<uint64_t>(ke.start_time.get_time_t()));
      _kpi_event_link.bind_value_as_u32(2, ke.ba_id);

      std::promise<uint64_t> result;
      _mysql.run_statement_and_get_int<uint64_t>(
          _kpi_event_link, &result, mysql_task::LAST_INSERT_ID, thread_id);

      uint64_t evt_id{
          result.get_future()
              .get()};  //_kpi_event_link.last_insert_id().toUInt()};
      _last_inserted_kpi[ke.ba_id].insert({ke.start_time.get_time_t(), evt_id});
    }
  } catch (std::exception const& e) {
    throw msg_fmt(
        "BAM-BI: could not update KPI {} starting at {}"
        " and ending at {}: {}",
        ke.kpi_id, ke.start_time, ke.end_time, e.what());
  }
}

/**
 *  Process a dimension ba and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_ba(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_ba_event const& dba =
      *std::static_pointer_cast<bam::dimension_ba_event const>(e);
  log_v2::bam()->debug("BAM-BI: processing declaration of BA {} ('{}')",
                       dba.ba_id, dba.ba_description);
  _dimension_ba_insert.bind_value_as_i32(0, dba.ba_id);
  _dimension_ba_insert.bind_value_as_str(
      1, misc::string::truncate(
             dba.ba_name,
             get_mod_bam_reporting_ba_col_size(mod_bam_reporting_ba_ba_name)));
  _dimension_ba_insert.bind_value_as_str(
      2, misc::string::truncate(dba.ba_description,
                                get_mod_bam_reporting_ba_col_size(
                                    mod_bam_reporting_ba_ba_description)));
  _dimension_ba_insert.bind_value_as_f64(3, dba.sla_month_percent_crit);
  _dimension_ba_insert.bind_value_as_f64(4, dba.sla_month_percent_warn);
  _dimension_ba_insert.bind_value_as_f64(5, dba.sla_duration_crit);
  _dimension_ba_insert.bind_value_as_f64(6, dba.sla_duration_warn);
  _mysql.run_statement(_dimension_ba_insert, database::mysql_error::insert_ba,
                       false);
}

/**
 *  Process a dimension bv and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_bv(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_bv_event const& dbv =
      *std::static_pointer_cast<bam::dimension_bv_event const>(e);
  log_v2::bam()->debug("BAM-BI: processing declaration of BV {} ('{}')",
                       dbv.bv_id, dbv.bv_name);

  _dimension_bv_insert.bind_value_as_i32(0, dbv.bv_id);
  _dimension_bv_insert.bind_value_as_str(
      1, misc::string::truncate(
             dbv.bv_name,
             get_mod_bam_reporting_bv_col_size(mod_bam_reporting_bv_bv_name)));
  _dimension_bv_insert.bind_value_as_str(
      2, misc::string::truncate(dbv.bv_description,
                                get_mod_bam_reporting_bv_col_size(
                                    mod_bam_reporting_bv_bv_description)));
  _mysql.run_statement(_dimension_bv_insert, database::mysql_error::insert_bv,
                       false);
}

/**
 *  Process a dimension ba bv relation and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_ba_bv_relation(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_ba_bv_relation_event const& dbabv =
      *std::static_pointer_cast<bam::dimension_ba_bv_relation_event const>(e);
  log_v2::bam()->debug("BAM-BI: processing relation between BA {} and BV {}",
                       dbabv.ba_id, dbabv.bv_id);

  _dimension_ba_bv_relation_insert.bind_value_as_i32(0, dbabv.ba_id);
  _dimension_ba_bv_relation_insert.bind_value_as_i32(1, dbabv.bv_id);
  _mysql.run_statement(_dimension_ba_bv_relation_insert,
                       database::mysql_error::insert_dimension_ba_bv, false);
}

/**
 *  Cache a dimension event, and commit it on the disk accordingly.
 *
 *  @param e  The event to process.
 */
void reporting_stream::_process_dimension(const std::shared_ptr<io::data>& e) {
  if (_processing_dimensions) {
    // Cache the event until the end of the dimensions dump.
    switch (e->type()) {
      case io::events::data_type<io::events::bam,
                                 bam::de_dimension_ba_event>::value: {
        bam::dimension_ba_event const& dba =
            *std::static_pointer_cast<bam::dimension_ba_event const>(e);
        log_v2::bam()->debug("BAM-BI: preparing ba dimension {} ('{}' '{}')",
                             dba.ba_id, dba.ba_name, dba.ba_description);
      } break;
      case io::events::data_type<io::events::bam,
                                 bam::de_dimension_bv_event>::value: {
        bam::dimension_bv_event const& dbv =
            *std::static_pointer_cast<bam::dimension_bv_event const>(e);
        log_v2::bam()->debug("BAM-BI: preparing bv dimension {} ('{}')",
                             dbv.bv_id, dbv.bv_name);
      } break;
      case io::events::data_type<
          io::events::bam, bam::de_dimension_ba_bv_relation_event>::value: {
        bam::dimension_ba_bv_relation_event const& dbabv =
            *std::static_pointer_cast<
                bam::dimension_ba_bv_relation_event const>(e);
        log_v2::bam()->debug(
            "BAM-BI: preparing relation between ba {} and bv {}", dbabv.ba_id,
            dbabv.bv_id);
      } break;
      case io::events::data_type<io::events::bam,
                                 bam::de_dimension_kpi_event>::value: {
        bam::dimension_kpi_event const& dk{
            *std::static_pointer_cast<bam::dimension_kpi_event const>(e)};
        std::string kpi_name;
        if (!dk.service_description.empty())
          kpi_name =
              fmt::format("svc: {} {}", dk.host_name, dk.service_description);
        else if (!dk.kpi_ba_name.empty())
          kpi_name = fmt::format("ba: {}", dk.kpi_ba_name);
        else if (!dk.boolean_name.empty())
          kpi_name = fmt::format("bool: {}", dk.boolean_name);
        else if (!dk.meta_service_name.empty())
          kpi_name = fmt::format("meta: {}", dk.meta_service_name);
        log_v2::bam()->debug("BAM-BI: preparing declaration of kpi {} ('{}')",
                             dk.kpi_id, kpi_name);
      } break;
      case io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod>::value: {
        bam::dimension_timeperiod const& tp =
            *std::static_pointer_cast<bam::dimension_timeperiod const>(e);
        log_v2::bam()->debug(
            "BAM-BI: preparing declaration of timeperiod {} ('{}')", tp.id,
            tp.name);
      } break;
      case io::events::data_type<
          io::events::bam, bam::de_dimension_timeperiod_exception>::value: {
        bam::dimension_timeperiod_exception const& tpe =
            *std::static_pointer_cast<
                bam::dimension_timeperiod_exception const>(e);
        log_v2::bam()->debug("BAM-BI: preparing exception of timeperiod {}",
                             tpe.timeperiod_id);
      } break;
      case io::events::data_type<
          io::events::bam, bam::de_dimension_timeperiod_exclusion>::value: {
        bam::dimension_timeperiod_exclusion const& tpe =
            *std::static_pointer_cast<
                bam::dimension_timeperiod_exclusion const>(e);
        log_v2::bam()->debug(
            "BAM-BI: preparing exclusion of timeperiod {} by timeperiod {}",
            tpe.excluded_timeperiod_id, tpe.timeperiod_id);
      } break;
      case io::events::data_type<
          io::events::bam, bam::de_dimension_ba_timeperiod_relation>::value: {
        bam::dimension_ba_timeperiod_relation const& r =
            *std::static_pointer_cast<
                bam::dimension_ba_timeperiod_relation const>(e);
        log_v2::bam()->debug(
            "BAM-BI: preparing relation of BA {} to timeperiod {}", r.ba_id,
            r.timeperiod_id);
      } break;
      default:
        log_v2::bam()->debug("BAM-BI: preparing event of type {:x}", e->type());
        break;
    }
    _dimension_data_cache.emplace_back(e);

  } else
    log_v2::bam()->warn(
        "Dimension of type {:x} not handled because dimension block not "
        "opened.",
        e->type());
}

/**
 *  Dispatch a dimension event.
 *
 *  @param[in] data  The dimension event.
 */
void reporting_stream::_dimension_dispatch(
    std::shared_ptr<io::data> const& data) {
  switch (data->type()) {
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_ba_event>::value:
      _process_dimension_ba(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_bv_event>::value:
      _process_dimension_bv(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_ba_bv_relation_event>::value:
      _process_dimension_ba_bv_relation(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_kpi_event>::value:
      _process_dimension_kpi(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_timeperiod>::value:
      _process_dimension_timeperiod(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_timeperiod_exception>::value:
      _process_dimension_timeperiod_exception(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_timeperiod_exclusion>::value:
      _process_dimension_timeperiod_exclusion(data);
      break;
    case io::events::data_type<io::events::bam,
                               bam::de_dimension_ba_timeperiod_relation>::value:
      _process_dimension_ba_timeperiod_relation(data);
      break;
    default:
      break;
  }
}

/**
 *  Process a dimension truncate signal and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_truncate_signal(
    const std::shared_ptr<io::data>& e) {
  const dimension_truncate_table_signal& dtts =
      *std::static_pointer_cast<const dimension_truncate_table_signal>(e);

  if (dtts.update_started) {
    _processing_dimensions = true;
    log_v2::bam()->debug(
        "BAM-BI: processing table truncation signal (opening)");

    _dimension_data_cache.clear();
  } else {
    log_v2::bam()->debug(
        "BAM-BI: processing table truncation signal (closing)");
    // Lock the availability thread.
    std::lock_guard<availability_thread> lock(*_availabilities);

    for (auto& stmt : _dimension_truncate_tables)
      _mysql.run_statement(stmt,
                           database::mysql_error::truncate_dimension_table);
    _timeperiods.clear();

    // XXX : dimension event acknowledgement might not work !!!
    //       For this reason, ignore any db error. We wouldn't
    //       be able to manage it on a stream level.
    try {
      for (auto& e : _dimension_data_cache)
        _dimension_dispatch(e);
    } catch (std::exception const& e) {
      logging::error(logging::medium)
          << "BAM-BI: ignored dimension insertion failure: " << e.what();
    }

    _mysql.commit();
    _dimension_data_cache.clear();
    _processing_dimensions = false;
  }
}

/**
 *  Process a dimension KPI and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_kpi(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_kpi_event const& dk{
      *std::static_pointer_cast<bam::dimension_kpi_event const>(e)};
  std::string kpi_name;
  if (!dk.service_description.empty())
    kpi_name.append(dk.host_name).append(" ").append(dk.service_description);
  else if (!dk.kpi_ba_name.empty())
    kpi_name = dk.kpi_ba_name;
  else if (!dk.boolean_name.empty())
    kpi_name = dk.boolean_name;
  else if (!dk.meta_service_name.empty())
    kpi_name = dk.meta_service_name;
  log_v2::bam()->debug("BAM-BI: processing declaration of KPI {} ('{}')",
                       dk.kpi_id, kpi_name);

  _dimension_kpi_insert.bind_value_as_i32(0, dk.kpi_id);
  _dimension_kpi_insert.bind_value_as_str(
      1, misc::string::truncate(kpi_name, get_mod_bam_reporting_kpi_col_size(
                                              mod_bam_reporting_kpi_kpi_name)));
  _dimension_kpi_insert.bind_value_as_i32(2, dk.ba_id);
  _dimension_kpi_insert.bind_value_as_str(
      3,
      misc::string::truncate(dk.ba_name, get_mod_bam_reporting_kpi_col_size(
                                             mod_bam_reporting_kpi_ba_name)));
  _dimension_kpi_insert.bind_value_as_i32(4, dk.host_id);
  _dimension_kpi_insert.bind_value_as_str(
      5, misc::string::truncate(dk.host_name,
                                get_mod_bam_reporting_kpi_col_size(
                                    mod_bam_reporting_kpi_host_name)));
  _dimension_kpi_insert.bind_value_as_i32(6, dk.service_id);
  _dimension_kpi_insert.bind_value_as_str(
      7,
      misc::string::truncate(dk.service_description,
                             get_mod_bam_reporting_kpi_col_size(
                                 mod_bam_reporting_kpi_service_description)));
  if (dk.kpi_ba_id)
    _dimension_kpi_insert.bind_value_as_i32(8, dk.kpi_ba_id);
  else
    _dimension_kpi_insert.bind_value_as_null(8);
  _dimension_kpi_insert.bind_value_as_str(
      9, misc::string::truncate(dk.kpi_ba_name,
                                get_mod_bam_reporting_kpi_col_size(
                                    mod_bam_reporting_kpi_kpi_ba_name)));
  _dimension_kpi_insert.bind_value_as_i32(10, dk.meta_service_id);
  _dimension_kpi_insert.bind_value_as_str(
      11, misc::string::truncate(dk.meta_service_name,
                                 get_mod_bam_reporting_kpi_col_size(
                                     mod_bam_reporting_kpi_meta_service_name)));
  _dimension_kpi_insert.bind_value_as_f64(12, dk.impact_warning);
  _dimension_kpi_insert.bind_value_as_f64(13, dk.impact_critical);
  _dimension_kpi_insert.bind_value_as_f64(14, dk.impact_unknown);
  _dimension_kpi_insert.bind_value_as_i32(15, dk.boolean_id);
  _dimension_kpi_insert.bind_value_as_str(
      16, misc::string::truncate(dk.boolean_name,
                                 get_mod_bam_reporting_kpi_col_size(
                                     mod_bam_reporting_kpi_boolean_name)));

  _mysql.run_statement(_dimension_kpi_insert,
                       database::mysql_error::insert_dimension_kpi, false);
}

/**
 *  Process a dimension timeperiod and store it in the DB and in the
 *  timeperiod cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod const& tp =
      *std::static_pointer_cast<bam::dimension_timeperiod const>(e);
  log_v2::bam()->debug("BAM-BI: processing declaration of timeperiod {} ('{}')",
                       tp.id, tp.name);

  _dimension_timeperiod_insert.bind_value_as_i32(0, tp.id);
  _dimension_timeperiod_insert.bind_value_as_str(
      1, misc::string::truncate(tp.name,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_name)));
  _dimension_timeperiod_insert.bind_value_as_str(
      2, misc::string::truncate(tp.sunday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_sunday)));
  _dimension_timeperiod_insert.bind_value_as_str(
      3, misc::string::truncate(tp.monday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_monday)));
  _dimension_timeperiod_insert.bind_value_as_str(
      4, misc::string::truncate(tp.tuesday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_tuesday)));
  _dimension_timeperiod_insert.bind_value_as_str(
      5, misc::string::truncate(tp.wednesday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_wednesday)));
  _dimension_timeperiod_insert.bind_value_as_str(
      6, misc::string::truncate(tp.thursday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_thursday)));
  _dimension_timeperiod_insert.bind_value_as_str(
      7, misc::string::truncate(tp.friday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_friday)));
  _dimension_timeperiod_insert.bind_value_as_str(
      8, misc::string::truncate(tp.saturday,
                                get_mod_bam_reporting_timeperiods_col_size(
                                    mod_bam_reporting_timeperiods_saturday)));
  _mysql.run_statement(_dimension_timeperiod_insert,
                       database::mysql_error::insert_timeperiod, false);
  _apply(tp);
}

/**
 *  Process a timeperiod exception and store it in the DB and in the
 *  timeperiod cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod_exception(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod_exception const& tpe =
      *std::static_pointer_cast<bam::dimension_timeperiod_exception const>(e);
  log_v2::bam()->debug("BAM-BI: processing exception of timeperiod {}",
                       tpe.timeperiod_id);

  _dimension_timeperiod_exception_insert.bind_value_as_i32(0,
                                                           tpe.timeperiod_id);
  _dimension_timeperiod_exception_insert.bind_value_as_str(
      1, misc::string::truncate(
             tpe.daterange,
             get_mod_bam_reporting_timeperiods_exceptions_col_size(
                 mod_bam_reporting_timeperiods_exceptions_daterange)));
  _dimension_timeperiod_exception_insert.bind_value_as_str(
      2, misc::string::truncate(
             tpe.timerange,
             get_mod_bam_reporting_timeperiods_exceptions_col_size(
                 mod_bam_reporting_timeperiods_exceptions_timerange)));

  _mysql.run_statement(_dimension_timeperiod_exception_insert,
                       database::mysql_error::insert_timeperiod_exception,
                       false);
  _apply(tpe);
}

/**
 *  Process a timeperiod exclusion and store it in the DB and in the
 *  timeperiod cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod_exclusion(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod_exclusion const& tpe =
      *std::static_pointer_cast<bam::dimension_timeperiod_exclusion const>(e);
  log_v2::bam()->debug(
      "BAM-BI: processing exclusion of timeperiod {} by timeperiod {}",
      tpe.excluded_timeperiod_id, tpe.timeperiod_id);

  _dimension_timeperiod_exclusion_insert.bind_value_as_i32(0,
                                                           tpe.timeperiod_id);
  _dimension_timeperiod_exclusion_insert.bind_value_as_i32(
      1, tpe.excluded_timeperiod_id);
  _mysql.run_statement(_dimension_timeperiod_exclusion_insert,
                       database::mysql_error::insert_exclusion_timeperiod,
                       false);
  _apply(tpe);
}

/**
 *  Process a dimension ba timeperiod relation and store it in
 *  a relation cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_ba_timeperiod_relation(
    std::shared_ptr<io::data> const& e) {
  bam::dimension_ba_timeperiod_relation const& r =
      *std::static_pointer_cast<bam::dimension_ba_timeperiod_relation const>(e);
  log_v2::bam()->debug("BAM-BI: processing relation of BA {} to timeperiod {}",
                       r.ba_id, r.timeperiod_id);

  _dimension_ba_timeperiod_insert.bind_value_as_i32(0, r.ba_id);
  _dimension_ba_timeperiod_insert.bind_value_as_i32(1, r.timeperiod_id);
  _dimension_ba_timeperiod_insert.bind_value_as_bool(2, r.is_default);
  _mysql.run_statement(_dimension_ba_timeperiod_insert,
                       database::mysql_error::insert_relation_ba_timeperiod,
                       false);
  _timeperiods.add_relation(r.ba_id, r.timeperiod_id, r.is_default);
}

/**
 *  @brief Compute and write the duration events associated with a ba event.
 *
 *  The event durations are computed from the associated timeperiods of the
 * BA.
 *
 *  @param[in] ev       The ba_event generating the durations.
 *  @param[in] visitor  A visitor stream.
 */
void reporting_stream::_compute_event_durations(
    std::shared_ptr<ba_event> const& ev,
    io::stream* visitor) {
  if (!ev || !visitor)
    return;

  log_v2::bam()->info(
      "BAM-BI: computing durations of event started at {} and ended at {} on "
      "BA {}",
      ev->start_time, ev->end_time, ev->ba_id);

  // Find the timeperiods associated with this ba.
  std::vector<std::pair<time::timeperiod::ptr, bool>> timeperiods =
      _timeperiods.get_timeperiods_by_ba_id(ev->ba_id);

  if (timeperiods.empty()) {
    log_v2::bam()->debug(
        "BAM-BI: no reporting period defined for event started at {} and ended "
        "at {} on BA {}",
        ev->start_time, ev->end_time, ev->ba_id);
    return;
  }

  for (std::vector<std::pair<time::timeperiod::ptr, bool>>::const_iterator
           it(timeperiods.begin()),
       end(timeperiods.end());
       it != end; ++it) {
    time::timeperiod::ptr tp = it->first;
    bool is_default = it->second;

    std::shared_ptr<ba_duration_event> dur_ev(new ba_duration_event);
    dur_ev->ba_id = ev->ba_id;
    dur_ev->real_start_time = ev->start_time;
    dur_ev->start_time = tp->get_next_valid(ev->start_time);
    dur_ev->end_time = ev->end_time;
    if ((dur_ev->start_time != (time_t)-1) &&
        (dur_ev->end_time != (time_t)-1) &&
        (dur_ev->start_time < dur_ev->end_time)) {
      dur_ev->duration = dur_ev->end_time - dur_ev->start_time;
      dur_ev->sla_duration =
          tp->duration_intersect(dur_ev->start_time, dur_ev->end_time);
      dur_ev->timeperiod_id = tp->get_id();
      dur_ev->timeperiod_is_default = is_default;
      log_v2::bam()->debug(
          "BAM-BI: durations of event started at {} and ended at {} on BA {} "
          "were computed for timeperiod {}, duration is {}s, SLA duration is "
          "{}",
          ev->start_time, ev->end_time, ev->ba_id, tp->get_name(),
          dur_ev->duration, dur_ev->sla_duration);
      visitor->write(std::static_pointer_cast<io::data>(dur_ev));
    } else
      log_v2::bam()->debug(
          "BAM-BI: event started at {} and ended at {} on BA {} has no "
          "duration on timeperiod {}",
          ev->start_time, ev->end_time, ev->ba_id, tp->get_name());
  }
}

/**
 *  Process a rebuild signal: Delete the obsolete data in the db and rebuild
 *  ba duration events.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_rebuild(std::shared_ptr<io::data> const& e) {
  const rebuild& r = *std::static_pointer_cast<const rebuild>(e);
  if (r.bas_to_rebuild.empty())
    return;
  log_v2::bam()->debug("BAM-BI: processing rebuild signal");

  _update_status("rebuilding: querying ba events");

  // We block the availability thread to prevent it waking
  // up on truncated event durations.
  try {
    std::lock_guard<availability_thread> lock(*_availabilities);

    // Delete obsolete ba events durations.
    {
      std::string query(
          fmt::format("DELETE a FROM mod_bam_reporting_ba_events_durations as "
                      "a INNER JOIN mod_bam_reporting_ba_events as b ON "
                      "a.ba_event_id = b.ba_event_id WHERE b.ba_id IN ({})",
                      r.bas_to_rebuild));

      _mysql.run_query(query, database::mysql_error::delete_ba_durations, true);
    }

    // Get the ba events.
    std::vector<std::shared_ptr<ba_event>> ba_events;
    {
      std::string query(
          fmt::format("SELECT ba_id, start_time, end_time, status, in_downtime "
                      "boolean FROM mod_bam_reporting_ba_events WHERE end_time "
                      "IS NOT NULL AND ba_id IN ({})",
                      r.bas_to_rebuild));
      std::promise<mysql_result> promise;
      _mysql.run_query_and_get_result(query, &promise);
      try {
        mysql_result res(promise.get_future().get());

        while (_mysql.fetch_row(res)) {
          std::shared_ptr<ba_event> baev(new ba_event);
          baev->ba_id = res.value_as_i32(0);
          baev->start_time = res.value_as_i32(1);
          baev->end_time = res.value_as_i32(2);
          baev->status = res.value_as_i32(3);
          baev->in_downtime = res.value_as_bool(4);
          ba_events.push_back(baev);
          log_v2::bam()->debug("BAM-BI: got events of BA {}", baev->ba_id);
        }
      } catch (std::exception const& e) {
        throw msg_fmt("BAM-BI: could not get BA events of {} : {}",
                      r.bas_to_rebuild, e.what());
      }
    }

    log_v2::bam()->info("BAM-BI: will now rebuild the event durations");

    size_t ba_events_num = ba_events.size();
    size_t ba_events_curr = 0;

    // Generate new ba events durations for each ba events.
    {
      for (auto it = ba_events.begin(), end = ba_events.end(); it != end;
           ++it, ++ba_events_curr) {
        std::string s(fmt::format("rebuilding: ba event {}/{}", ba_events_curr,
                                  ba_events_num));
        _update_status(s);
        _compute_event_durations(*it, this);
      }
    }
  } catch (...) {
    _update_status("");
    throw;
  }

  log_v2::bam()->info(
      "BAM-BI: event durations rebuild finished, will rebuild availabilities "
      "now");

  // Ask for the availabilities thread to recompute the availabilities.
  _availabilities->rebuild_availabilities(r.bas_to_rebuild);

  _update_status("");
}

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void reporting_stream::_update_status(std::string const& status) {
  std::lock_guard<std::mutex> lock(_statusm);
  _status = status;
}
