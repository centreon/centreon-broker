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
#include <sstream>
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
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/time/timezone_manager.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;
using namespace com::centreon::broker::database;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_cfg                  BAM DB configuration.
 */
reporting_stream::reporting_stream(database_config const& db_cfg)
    : _ack_events(0), _pending_events(0), _mysql(db_cfg) {
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
  throw exceptions::shutdown() << "cannot read from BAM reporting stream";
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
    tree["status"] =  _status;
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

  if (data->type() ==
      io::events::data_type<io::events::bam, bam::de_kpi_event>::value)
    _process_kpi_event(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam, bam::de_ba_event>::value)
    _process_ba_event(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_ba_duration_event>::value)
    _process_ba_duration_event(data);
  else if (data->type() ==
               io::events::data_type<io::events::bam,
                                     bam::de_dimension_ba_event>::value ||
           data->type() ==
               io::events::data_type<io::events::bam,
                                     bam::de_dimension_bv_event>::value ||
           data->type() == io::events::data_type<
                               io::events::bam,
                               bam::de_dimension_ba_bv_relation_event>::value ||
           data->type() ==
               io::events::data_type<io::events::bam,
                                     bam::de_dimension_kpi_event>::value ||
           data->type() ==
               io::events::data_type<
                   io::events::bam,
                   bam::de_dimension_truncate_table_signal>::value ||
           data->type() ==
               io::events::data_type<io::events::bam,
                                     bam::de_dimension_timeperiod>::value ||
           data->type() == io::events::data_type<
                               io::events::bam,
                               bam::de_dimension_timeperiod_exception>::value ||
           data->type() == io::events::data_type<
                               io::events::bam,
                               bam::de_dimension_timeperiod_exclusion>::value ||
           data->type() == io::events::data_type<
                               io::events::bam,
                               bam::de_dimension_ba_timeperiod_relation>::value)
    _process_dimension(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam, bam::de_rebuild>::value)
    _process_rebuild(data);

  // Event acknowledgement.
  int retval(_ack_events);
  _ack_events = 0;
  return retval;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Apply a timeperiod declaration.
 *
 *  @param[in] tp  Timeperiod declaration.
 */
void reporting_stream::_apply(dimension_timeperiod const& tp) {
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
    std::ostringstream query;
    query << "SELECT e1." << id << ", e1.start_time"
          << "  FROM " << table << " As e1 INNER JOIN ("
          << "    SELECT " << id << ", MAX(start_time) AS max_start_time"
          << "      FROM " << table << "      GROUP BY " << id << ") AS e2"
          << "        ON e1." << id << "=e2." << id
          << "  WHERE e1.end_time IS NULL"
          << "    AND e1.start_time!=e2.max_start_time";
    std::promise<mysql_result> promise;
    _mysql.run_query_and_get_result(query.str(), &promise);
    try {
      mysql_result res(promise.get_future().get());
      while (_mysql.fetch_row(res))
        events.push_back(std::make_pair(
            res.value_as_u32(0), static_cast<time_t>(res.value_as_i32(1))));
    } catch (std::exception const& e) {
      throw exceptions::msg()
          << "BAM-BI: could not get inconsistent events: " << e.what();
    }
  }

  // Close each event.
  for (std::list<std::pair<uint32_t, time_t>>::const_iterator
           it(events.begin()),
       end(events.end());
       it != end; ++it) {
    time_t end_time;
    {
      std::ostringstream oss;
      oss << "SELECT start_time"
          << "  FROM " << table << "  WHERE " << id << "=" << it->first
          << "    AND start_time>" << it->second << "  ORDER BY start_time ASC"
          << "  LIMIT 1";

      std::ostringstream oss_err;
      std::promise<mysql_result> promise;
      _mysql.run_query_and_get_result(oss.str(), &promise);
      try {
        mysql_result res(promise.get_future().get());
        if (!_mysql.fetch_row(res))
          throw(exceptions::msg() << "no event following this one");

        end_time = res.value_as_i32(0);
      } catch (std::exception const& e) {
        throw exceptions::msg()
            << "BAM-BI: could not get end time of inconsistent event of "
            << event_type << " " << it->first << " starting at " << it->second
            << ": " << e.what();
      }
    }
    {
      std::ostringstream oss;
      oss << "UPDATE " << table << "  SET end_time=" << end_time << "  WHERE "
          << id << "=" << it->first << "  AND start_time=" << it->second;

      std::ostringstream oss_err;
      oss_err << "BAM-BI: could not close inconsistent event of " << event_type
              << it->first << " starting at " << it->second << ": ";
      _mysql.run_query(oss.str(), oss_err.str(), true);
    }
  }
}

void reporting_stream::_close_all_events() {
  time_t now(::time(nullptr));
  std::ostringstream query;

  query << "UPDATE mod_bam_reporting_ba_events"
           " SET end_time="
        << now << " WHERE end_time IS NULL";
  _mysql.run_query(query.str(), "BAM-BI: could not close all ba events");

  query.str("");
  query << "UPDATE mod_bam_reporting_kpi_events"
           "  SET end_time="
        << now << "  WHERE end_time IS NULL";
  _mysql.run_query(query.str(), "BAM-BI, could not close all kpi events");
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
        "SELECT timeperiod_id, name, sunday, monday, tuesday,"
        "       wednesday, thursday, friday, saturday"
        "  FROM mod_bam_reporting_timeperiods");
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
      throw exceptions::msg()
          << "BAM-BI: could not load timeperiods from DB: " << e.what();
    }
  }

  // Load exceptions.
  {
    std::string query(
        "SELECT timeperiod_id, daterange, timerange"
        "  FROM mod_bam_reporting_timeperiods_exceptions");
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
      throw exceptions::msg()
          << "BAM-BI: could not load timeperiods exceptions from DB: "
          << e.what();
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
      throw exceptions::msg()
          << "BAM-BI: could not load exclusions from DB: " << e.what();
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
      throw exceptions::msg()
          << "BAM-BI: could not load BA/timeperiods relations: " << e.what();
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
      "           (ba_event_id, kpi_event_id)"
      "  SELECT be.ba_event_id, ke.kpi_event_id"
      "    FROM mod_bam_reporting_kpi_events AS ke"
      "    INNER JOIN mod_bam_reporting_ba_events AS be"
      "    ON ((ke.start_time >= be.start_time)"
      "       AND (be.end_time IS NULL OR ke.start_time < be.end_time))"
      "    INNER JOIN mod_bam_reporting_kpi AS rki"
      "     ON (rki.ba_id = be.ba_id AND rki.kpi_id = ke.kpi_id)"
      "    WHERE ke.kpi_id=? AND ke.start_time=?";
  _kpi_event_link = _mysql.prepare_query(query);

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
  logging::debug(logging::low)
      << "BAM-BI: processing event of BA " << be.ba_id << " (start time "
      << be.start_time << ", end time " << be.end_time << ", status "
      << be.status << ", in downtime " << be.in_downtime << ")";

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
  _mysql.run_statement_and_get_int(_ba_event_update, &promise,
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

      std::ostringstream oss_err;
      oss_err << "BAM-BI: could not insert event of BA " << be.ba_id
              << " starting at " << be.start_time << ": ";
      _mysql.run_statement(_ba_full_event_insert, oss_err.str(), true);
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "BAM-BI: could not update event of BA " << be.ba_id
        << " starting at " << be.start_time << " and ending at " << be.end_time
        << ": " << e.what();
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
  logging::debug(logging::low)
      << "BAM-BI: processing BA duration event of BA " << bde.ba_id
      << " (start time " << bde.start_time << ", end time " << bde.end_time
      << ", duration " << bde.duration << ", sla duration " << bde.sla_duration
      << ")";

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
  int thread_id(
      _mysql.run_statement_and_get_int(_ba_duration_event_update, &promise,
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

      _mysql.run_statement(_ba_duration_event_insert, "Insertion failed", true,
                           thread_id);
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "BAM-BI: could not insert duration event of BA " << bde.ba_id
        << " starting at " << bde.start_time << ": " << e.what();
  }
}

/**
 *  Process a kpi event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_kpi_event(std::shared_ptr<io::data> const& e) {
  bam::kpi_event const& ke = *std::static_pointer_cast<bam::kpi_event const>(e);
  logging::debug(logging::low)
      << "BAM-BI: processing event of KPI " << ke.kpi_id << " (start time "
      << ke.start_time << ", end time " << ke.end_time << ", state "
      << ke.status << ", in downtime " << ke.in_downtime << ")";

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
  int thread_id(_mysql.run_statement_and_get_int(
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

      std::ostringstream oss_err;
      oss_err << "BAM-BI: could not insert event of KPI " << ke.kpi_id
              << " starting at " << ke.start_time << " and ending at "
              << ke.end_time << ": ";
      _mysql.run_statement(_kpi_full_event_insert, oss_err.str(), true,
                           thread_id);

      // Insert kpi event link.
      _kpi_event_link.bind_value_as_i32(0, ke.kpi_id);
      _kpi_event_link.bind_value_as_u64(
          1, static_cast<uint64_t>(ke.start_time.get_time_t()));
      oss_err.str("");
      oss_err << "BAM-BI: could not create link from event of KPI " << ke.kpi_id
              << " starting at " << ke.start_time
              << " to its associated BA event: ";
      _mysql.run_statement(_kpi_event_link, oss_err.str(), true, thread_id);
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "BAM-BI: could not update KPI " << ke.kpi_id << " starting at "
        << ke.start_time << " and ending at " << ke.end_time << ": "
        << e.what();
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
  logging::debug(logging::low)
      << "BAM-BI: processing declaration of BA " << dba.ba_id << " ('"
      << dba.ba_description << "')";
  _dimension_ba_insert.bind_value_as_i32(0, dba.ba_id);
  _dimension_ba_insert.bind_value_as_str(1, dba.ba_name);
  _dimension_ba_insert.bind_value_as_str(2, dba.ba_description);
  _dimension_ba_insert.bind_value_as_f64(3, dba.sla_month_percent_crit);
  _dimension_ba_insert.bind_value_as_f64(4, dba.sla_month_percent_warn);
  _dimension_ba_insert.bind_value_as_f64(5, dba.sla_duration_crit);
  _dimension_ba_insert.bind_value_as_f64(6, dba.sla_duration_warn);
  std::ostringstream oss_err;
  oss_err << "BAM-BI: could not insert BA " << dba.ba_id << ": ";
  _mysql.run_statement(_dimension_ba_insert, oss_err.str(), true);
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
  logging::debug(logging::low) << "BAM-BI: processing declaration of BV "
                               << dbv.bv_id << " ('" << dbv.bv_name << "')";

  _dimension_bv_insert.bind_value_as_i32(0, dbv.bv_id);
  _dimension_bv_insert.bind_value_as_str(1, dbv.bv_name);
  _dimension_bv_insert.bind_value_as_str(2, dbv.bv_description);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert BV " << dbv.bv_id << ": ";
  _mysql.run_statement(_dimension_bv_insert, oss.str(), true);
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
  logging::debug(logging::low) << "BAM-BI: processing relation between BA "
                               << dbabv.ba_id << " and BV " << dbabv.bv_id;

  _dimension_ba_bv_relation_insert.bind_value_as_i32(0, dbabv.ba_id);
  _dimension_ba_bv_relation_insert.bind_value_as_i32(1, dbabv.bv_id);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert dimension of BA-BV relation " << dbabv.ba_id
      << "-" << dbabv.bv_id << ": ";
  _mysql.run_statement(_dimension_ba_bv_relation_insert, oss.str(), true);
}

/**
 *  Cache a dimension event, and commit it on the disk accordingly.
 *
 *  @param e  The event to process.
 */
void reporting_stream::_process_dimension(std::shared_ptr<io::data> const& e) {
  // Cache the event until the end of the dimensions dump.
  _dimension_data_cache.push_back(_dimension_copy(e));

  // If this is a dimension truncate table signal, it's either the beginning
  // or the end of the dimensions dump.
  if (e->type() ==
      io::events::data_type<io::events::bam,
                            bam::de_dimension_truncate_table_signal>::value) {
    dimension_truncate_table_signal const& dtts =
        *std::static_pointer_cast<dimension_truncate_table_signal const>(e);

    if (!dtts.update_started) {
      // Lock the availability thread.
      std::unique_ptr<std::unique_lock<std::mutex>> lock(
          _availabilities->lock());

      // XXX : dimension event acknowledgement might not work !!!
      //       For this reason, ignore any db error. We wouldn't
      //       be able to manage it on a stream level.
      try {
        for (std::vector<std::shared_ptr<io::data>>::const_iterator
                 it(_dimension_data_cache.begin()),
             end(_dimension_data_cache.end());
             it != end; ++it)
          _dimension_dispatch(*it);
        _mysql.commit();
      } catch (std::exception const& e) {
        logging::error(logging::medium)
            << "BAM-BI: ignored dimension insertion failure: " << e.what();
      }

      _dimension_data_cache.clear();
    } else
      _dimension_data_cache.erase(_dimension_data_cache.begin(),
                                  _dimension_data_cache.end() - 1);
  }
}

/**
 *  Dispatch a dimension event.
 *
 *  @param[in] data  The dimension event.
 */
void reporting_stream::_dimension_dispatch(
    std::shared_ptr<io::data> const& data) {
  if (data->type() ==
      io::events::data_type<io::events::bam, bam::de_dimension_ba_event>::value)
    _process_dimension_ba(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_bv_event>::value)
    _process_dimension_bv(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_ba_bv_relation_event>::value)
    _process_dimension_ba_bv_relation(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_kpi_event>::value)
    _process_dimension_kpi(data);
  else if (data->type() ==
           io::events::data_type<
               io::events::bam, bam::de_dimension_truncate_table_signal>::value)
    _process_dimension_truncate_signal(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod>::value)
    _process_dimension_timeperiod(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod_exception>::value)
    _process_dimension_timeperiod_exception(data);
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod_exclusion>::value)
    _process_dimension_timeperiod_exclusion(data);
  else if (data->type() == io::events::data_type<
                               io::events::bam,
                               bam::de_dimension_ba_timeperiod_relation>::value)
    _process_dimension_ba_timeperiod_relation(data);
}

/**
 *  Copy a dimension event.
 *
 *  @param[in] data  The data to copy.
 *
 *  @return  The dimension event copied.
 */
std::shared_ptr<io::data> reporting_stream::_dimension_copy(
    std::shared_ptr<io::data> const& data) {
  if (data->type() ==
      io::events::data_type<io::events::bam, bam::de_dimension_ba_event>::value)
    return std::make_shared<bam::dimension_ba_event>(
        *std::static_pointer_cast<bam::dimension_ba_event>(data));
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_bv_event>::value)
    return std::make_shared<bam::dimension_bv_event>(
        *std::static_pointer_cast<bam::dimension_bv_event>(data));
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_ba_bv_relation_event>::value)
    return std::make_shared<bam::dimension_ba_bv_relation_event>(
        *std::static_pointer_cast<bam::dimension_ba_bv_relation_event>(data));
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_kpi_event>::value)
    return std::make_shared<bam::dimension_kpi_event>(
        *std::static_pointer_cast<bam::dimension_kpi_event>(data));
  else if (data->type() ==
           io::events::data_type<
               io::events::bam, bam::de_dimension_truncate_table_signal>::value)
    return std::make_shared<bam::dimension_truncate_table_signal>(
        *std::static_pointer_cast<bam::dimension_truncate_table_signal>(data));
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod>::value)
    return std::make_shared<bam::dimension_timeperiod>(
        *std::static_pointer_cast<bam::dimension_timeperiod>(data));
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod_exception>::value)
    return std::make_shared<bam::dimension_timeperiod_exception>(
        *std::static_pointer_cast<bam::dimension_timeperiod_exception>(data));
  else if (data->type() ==
           io::events::data_type<io::events::bam,
                                 bam::de_dimension_timeperiod_exclusion>::value)
    return std::make_shared<bam::dimension_timeperiod_exclusion>(
        *std::static_pointer_cast<bam::dimension_timeperiod_exclusion>(data));
  else if (data->type() == io::events::data_type<
                               io::events::bam,
                               bam::de_dimension_ba_timeperiod_relation>::value)
    return std::make_shared<bam::dimension_ba_timeperiod_relation>(
        *std::static_pointer_cast<bam::dimension_ba_timeperiod_relation>(data));
  return std::shared_ptr<io::data>();
}

/**
 *  Process a dimension truncate signal and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_truncate_signal(
    std::shared_ptr<io::data> const& e) {
  dimension_truncate_table_signal const& dtts =
      *std::static_pointer_cast<dimension_truncate_table_signal const>(e);

  if (dtts.update_started) {
    logging::debug(logging::low)
        << "BAM-BI: processing table truncation signal";

    for (std::vector<mysql_stmt>::iterator
             it(_dimension_truncate_tables.begin()),
         end(_dimension_truncate_tables.end());
         it != end; ++it)
      _mysql.run_statement(*it,
                           "BAM-BI: could not truncate some dimension table");

    _timeperiods.clear();
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
  logging::debug(logging::low) << "BAM-BI: processing declaration of KPI "
                               << dk.kpi_id << " ('" << kpi_name << "')";

  _dimension_kpi_insert.bind_value_as_i32(0, dk.kpi_id);
  _dimension_kpi_insert.bind_value_as_str(1, kpi_name);
  _dimension_kpi_insert.bind_value_as_i32(2, dk.ba_id);
  _dimension_kpi_insert.bind_value_as_str(3, dk.ba_name);
  _dimension_kpi_insert.bind_value_as_i32(5, dk.host_id);
  _dimension_kpi_insert.bind_value_as_str(6, dk.host_name);
  _dimension_kpi_insert.bind_value_as_i32(7, dk.service_id);
  _dimension_kpi_insert.bind_value_as_str(8, dk.service_description);
  if (dk.kpi_ba_id)
    _dimension_kpi_insert.bind_value_as_i32(9, dk.kpi_ba_id);
  else
    _dimension_kpi_insert.bind_value_as_null(10);
  _dimension_kpi_insert.bind_value_as_str(11, dk.kpi_ba_name);
  _dimension_kpi_insert.bind_value_as_i32(12, dk.meta_service_id);
  _dimension_kpi_insert.bind_value_as_str(13, dk.meta_service_name);
  _dimension_kpi_insert.bind_value_as_f64(14, dk.impact_warning);
  _dimension_kpi_insert.bind_value_as_f64(15, dk.impact_critical);
  _dimension_kpi_insert.bind_value_as_f64(16, dk.impact_unknown);
  _dimension_kpi_insert.bind_value_as_i32(17, dk.boolean_id);
  _dimension_kpi_insert.bind_value_as_str(18, dk.boolean_name);

  std::ostringstream oss;
  oss << "BAM-BI: could not insert dimension of KPI " << dk.kpi_id << ": ";
  _mysql.run_statement(_dimension_kpi_insert, oss.str(), true);
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
  logging::debug(logging::low)
      << "BAM-BI: processing declaration of timeperiod " << tp.id << " ('"
      << tp.name << "')";

  _dimension_timeperiod_insert.bind_value_as_i32(0, tp.id);
  _dimension_timeperiod_insert.bind_value_as_str(1, tp.name);
  _dimension_timeperiod_insert.bind_value_as_str(2, tp.sunday);
  _dimension_timeperiod_insert.bind_value_as_str(3, tp.monday);
  _dimension_timeperiod_insert.bind_value_as_str(4, tp.tuesday);
  _dimension_timeperiod_insert.bind_value_as_str(5, tp.wednesday);
  _dimension_timeperiod_insert.bind_value_as_str(6, tp.thursday);
  _dimension_timeperiod_insert.bind_value_as_str(7, tp.friday);
  _dimension_timeperiod_insert.bind_value_as_str(8, tp.saturday);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert timeperiod " << tp.id << " ('" << tp.name
      << "'): ";
  _mysql.run_statement(_dimension_timeperiod_insert, oss.str(), true);
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
  logging::debug(logging::low)
      << "BAM-BI: processing exception of timeperiod " << tpe.timeperiod_id;

  _dimension_timeperiod_exception_insert.bind_value_as_i32(0,
                                                           tpe.timeperiod_id);
  _dimension_timeperiod_exception_insert.bind_value_as_str(1, tpe.daterange);
  _dimension_timeperiod_exception_insert.bind_value_as_str(2, tpe.timerange);

  std::ostringstream oss;
  oss << "BAM-BI: could not insert exception of timeperiod "
      << tpe.timeperiod_id << ": ";
  _mysql.run_statement(_dimension_timeperiod_exception_insert, oss.str(), true);
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
  logging::debug(logging::low)
      << "BAM-BI: processing exclusion of timeperiod "
      << tpe.excluded_timeperiod_id << " by timeperiod " << tpe.timeperiod_id;

  _dimension_timeperiod_exclusion_insert.bind_value_as_i32(0,
                                                           tpe.timeperiod_id);
  _dimension_timeperiod_exclusion_insert.bind_value_as_i32(
      1, tpe.excluded_timeperiod_id);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert exclusion of timeperiod "
      << tpe.excluded_timeperiod_id << " by timeperiod " << tpe.timeperiod_id
      << ": ";
  _mysql.run_statement(_dimension_timeperiod_exclusion_insert, oss.str(), true);
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
  logging::debug(logging::low)
      << "BAM-BI: processing relation of BA " << r.ba_id << " to timeperiod "
      << r.timeperiod_id;

  _dimension_ba_timeperiod_insert.bind_value_as_i32(0, r.ba_id);
  _dimension_ba_timeperiod_insert.bind_value_as_i32(1, r.timeperiod_id);
  _dimension_ba_timeperiod_insert.bind_value_as_bool(2, r.is_default);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert relation of BA " << r.ba_id
      << " to timeperiod " << r.timeperiod_id << ": ";
  _mysql.run_statement(_dimension_ba_timeperiod_insert, oss.str(), true);
  _timeperiods.add_relation(r.ba_id, r.timeperiod_id, r.is_default);
}

/**
 *  @brief Compute and write the duration events associated with a ba event.
 *
 *  The event durations are computed from the associated timeperiods of the BA.
 *
 *  @param[in] ev       The ba_event generating the durations.
 *  @param[in] visitor  A visitor stream.
 */
void reporting_stream::_compute_event_durations(
    std::shared_ptr<ba_event> const& ev,
    io::stream* visitor) {
  if (!ev || !visitor)
    return;

  logging::info(logging::medium)
      << "BAM-BI: computing durations of event started at " << ev->start_time
      << " and ended at " << ev->end_time << " on BA " << ev->ba_id;

  // Find the timeperiods associated with this ba.
  std::vector<std::pair<time::timeperiod::ptr, bool>> timeperiods =
      _timeperiods.get_timeperiods_by_ba_id(ev->ba_id);

  if (timeperiods.empty()) {
    logging::debug(logging::medium)
        << "BAM-BI: no reporting period defined for event started at "
        << ev->start_time << " and ended at " << ev->end_time << " on BA "
        << ev->ba_id;
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
      logging::debug(logging::low)
          << "BAM-BI: durations of event started at " << ev->start_time
          << " and ended at " << ev->end_time << " on BA " << ev->ba_id
          << " were computed for timeperiod " << tp->get_name()
          << ", duration is " << dur_ev->duration << "s, SLA duration is "
          << dur_ev->sla_duration;
      visitor->write(std::static_pointer_cast<io::data>(dur_ev));
    } else
      logging::debug(logging::medium)
          << "BAM-BI: event started at " << ev->start_time << " and ended at "
          << ev->end_time << " on BA " << ev->ba_id
          << " has no duration on timeperiod " << tp->get_name();
  }
}

/**
 *  Process a rebuild signal: Delete the obsolete data in the db and rebuild
 *  ba duration events.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_rebuild(std::shared_ptr<io::data> const& e) {
  rebuild const& r = *std::static_pointer_cast<rebuild const>(e);
  if (r.bas_to_rebuild.empty())
    return;
  logging::debug(logging::low) << "BAM-BI: processing rebuild signal";

  _update_status("rebuilding: querying ba events");

  // We block the availability thread to prevent it waking
  // up on truncated event durations.
  try {
    std::unique_ptr<std::unique_lock<std::mutex>> lock(_availabilities->lock());

    // Delete obsolete ba events durations.
    {
      std::string query(
          "DELETE a"
          "  FROM mod_bam_reporting_ba_events_durations as a"
          "    INNER JOIN mod_bam_reporting_ba_events as b"
          "      ON a.ba_event_id = b.ba_event_id"
          "  WHERE b.ba_id IN (");
      query.append(r.bas_to_rebuild);
      query.append(")");

      std::ostringstream oss;
      oss << "BAM-BI: could not delete BA durations " << r.bas_to_rebuild
          << ": ";
      _mysql.run_query(query, oss.str(), true);
    }

    // Get the ba events.
    std::vector<std::shared_ptr<ba_event>> ba_events;
    {
      std::string query(
          "SELECT ba_id, start_time, end_time, "
          "       status, in_downtime boolean"
          "  FROM mod_bam_reporting_ba_events"
          "  WHERE end_time IS NOT NULL"
          "    AND ba_id IN (");
      query.append(r.bas_to_rebuild);
      query.append(")");
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
          logging::debug(logging::low)
              << "BAM-BI: got events of BA " << baev->ba_id;
        }
      } catch (std::exception const& e) {
        throw exceptions::msg() << "BAM-BI: could not get BA events of "
                                << r.bas_to_rebuild << ": " << e.what();
      }
    }

    logging::info(logging::medium)
        << "BAM-BI: will now rebuild the event durations";

    size_t ba_events_num = ba_events.size();
    size_t ba_events_curr = 0;
    std::stringstream ss;

    // Generate new ba events durations for each ba events.
    {
      for (std::vector<std::shared_ptr<ba_event>>::const_iterator
               it(ba_events.begin()),
           end(ba_events.end());
           it != end; ++it, ++ba_events_curr) {
        ss.str("");
        ss << "rebuilding: ba event " << ba_events_curr << "/" << ba_events_num;
        _update_status(ss.str());
        _compute_event_durations(*it, this);
      }
    }
  } catch (...) {
    _update_status("");
    throw;
  }

  logging::info(logging::medium) << "BAM-BI: event durations rebuild finished, "
                                    " will rebuild availabilities now";

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
