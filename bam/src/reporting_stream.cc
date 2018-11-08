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

#include <cstdlib>
#include <sstream>
#include <QMutexLocker>
#include <QVariant>
#include "com/centreon/broker/bam/ba_event.hh"
#include "com/centreon/broker/bam/ba_duration_event.hh"
#include "com/centreon/broker/bam/dimension_ba_bv_relation_event.hh"
#include "com/centreon/broker/bam/dimension_ba_event.hh"
#include "com/centreon/broker/bam/dimension_bv_event.hh"
#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/dimension_truncate_table_signal.hh"
#include "com/centreon/broker/bam/dimension_timeperiod.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
#include "com/centreon/broker/bam/dimension_timeperiod_exclusion.hh"
#include "com/centreon/broker/bam/dimension_ba_timeperiod_relation.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/rebuild.hh"
#include "com/centreon/broker/bam/reporting_stream.hh"
#include "com/centreon/broker/time/timezone_manager.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] db_type                 BAM DB type.
 *  @param[in] db_host                 BAM DB host.
 *  @param[in] db_port                 BAM DB port.
 *  @param[in] db_user                 BAM DB user.
 *  @param[in] db_password             BAM DB password.
 *  @param[in] db_name                 BAM DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] check_replication       true to check replication status.
 */
reporting_stream::reporting_stream(database_config const& db_cfg)
  : _pending_events(0),
    _ack_events(0),
    _mysql(db_cfg) {
  // Prepare queries.
  _prepare();

  // Load timeperiods.
  _load_timeperiods();

  // Close inconsistent events.
  _close_inconsistent_events(
    "BA",
    "mod_bam_reporting_ba_events",
    "ba_id");
  _close_inconsistent_events(
    "KPI",
    "mod_bam_reporting_kpi_events",
    "kpi_id");

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
bool reporting_stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (exceptions::shutdown()
         << "cannot read from BAM reporting stream");
  return (true);
}
/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void reporting_stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty())
    tree.add_property("status", io::property("status", _status));
  return ;
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
int reporting_stream::write(misc::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_events;
  if (!validate(data, "BAM-BI"))
    return (0);

  if (data->type()
      == io::events::data_type<io::events::bam,
                               bam::de_kpi_event>::value)
    _process_kpi_event(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_ba_event>::value)
    _process_ba_event(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_ba_duration_event>::value)
    _process_ba_duration_event(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_ba_event>::value ||
           data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_bv_event>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_ba_bv_relation_event>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_kpi_event>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_truncate_table_signal>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_timeperiod>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_timeperiod_exception>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_timeperiod_exclusion>::value ||
           data->type()
                        == io::events::data_type<io::events::bam,
                                                 bam::de_dimension_ba_timeperiod_relation>::value)
    _process_dimension(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_rebuild>::value)
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
  _timeperiods.add_timeperiod(tp.id, time::timeperiod::ptr(
    new time::timeperiod(
                tp.id,
                tp.name.toStdString(),
                "",
                tp.sunday.toStdString(),
                tp.monday.toStdString(),
                tp.tuesday.toStdString(),
                tp.wednesday.toStdString(),
                tp.thursday.toStdString(),
                tp.friday.toStdString(),
                tp.saturday.toStdString())));
  return ;
}

/**
 *  Apply a timeperiod exception declaration.
 *
 *  @param[in] tpe  Timeperiod exclusion declaration.
 */
void reporting_stream::_apply(
                         dimension_timeperiod_exception const& tpe) {
  time::timeperiod::ptr timeperiod =
      _timeperiods.get_timeperiod(tpe.timeperiod_id);
  if (timeperiod)
    timeperiod->add_exception(
                  tpe.daterange.toStdString(),
                  tpe.timerange.toStdString());
  else
    logging::error(logging::medium)
      << "BAM-BI: could not apply exception on timeperiod "
      << tpe.timeperiod_id << ": timeperiod does not exist";
  return ;
}

/**
 *  Apply a timeperiod exclusion declaration.
 *
 *  @param[in] tpe  Timeperiod exclusion declaration.
 */
void reporting_stream::_apply(
                         dimension_timeperiod_exclusion const& tpe) {
  time::timeperiod::ptr timeperiod =
      _timeperiods.get_timeperiod(tpe.timeperiod_id);
  time::timeperiod::ptr excluded_tp =
      _timeperiods.get_timeperiod(tpe.excluded_timeperiod_id);
  if (timeperiod && excluded_tp)
    timeperiod->add_excluded(excluded_tp);
  else
    logging::error(logging::medium)
      << "BAM-BI: could not apply exclusion of timeperiod "
      << tpe.excluded_timeperiod_id << " by timeperiod "
      << tpe.timeperiod_id
      << ": at least one of the timeperiod does not exist";
  return ;
}

/**
 *  Delete inconsistent events.
 *
 *  @param[in] event_type  Event type (BA or KPI).
 *  @param[in] table       Table name.
 *  @param[in] id          Table ID name.
 */
void reporting_stream::_close_inconsistent_events(
                         char const* event_type,
                         char const* table,
                         char const* id) {
  // Get events to close.
  std::list<std::pair<unsigned int, time_t> > events;
  {
    std::ostringstream query;
    query << "SELECT e1." << id << ", e1.start_time"
          << "  FROM " << table << " As e1 INNER JOIN ("
          << "    SELECT " << id << ", MAX(start_time) AS max_start_time"
          << "      FROM " << table
          << "      GROUP BY " << id << ") AS e2"
          << "        ON e1." << id << "=e2." << id
          << "  WHERE e1.end_time IS NULL"
          << "    AND e1.start_time!=e2.max_start_time";
    int thread_id(_mysql.run_query(
        query.str(),
        "BAM-BI: could not get inconsistent events"));
    mysql_result res(_mysql.get_result(thread_id));
    while (_mysql.fetch_row(thread_id, res))
      events.push_back(std::make_pair(
               res.value_as_u32(0),
               static_cast<time_t>(res.value_as_i32(1))));
  }

  // Close each event.
  for (std::list<std::pair<unsigned int, time_t> >::const_iterator
         it(events.begin()),
         end(events.end());
       it != end;
       ++it) {
    time_t end_time;
    {
      std::ostringstream oss;
      oss << "SELECT start_time"
          << "  FROM " << table
          << "  WHERE " << id << "=" << it->first
          << "    AND start_time>" << it->second
          << "  ORDER BY start_time ASC"
          << "  LIMIT 1";

      std::ostringstream oss_err;
      oss_err << "BAM-BI: could not get end time of inconsistent event of "
              << event_type << " " << it->first << " starting at "
              << it->second << ": ";
      int thread_id(_mysql.run_query(oss.str(), oss_err.str(), true));
      mysql_result res(_mysql.get_result(thread_id));
      if (!_mysql.fetch_row(thread_id, res))
        throw (exceptions::msg() << "no event following this one");

      end_time = res.value_as_i32(0);
    }
    {
      std::ostringstream oss;
      oss << "UPDATE " << table
          << "  SET end_time=" << end_time
          << "  WHERE " << id << "=" << it->first
          << "  AND start_time=" << it->second;
      std::ostringstream oss_err;
      oss_err << "BAM-BI: could not close inconsistent event of "
              << event_type << it->first << " starting at "
              << it->second << ": ";
      _mysql.run_query(oss.str(), oss_err.str(), true);
    }
  }
}

void reporting_stream::_close_all_events() {
  time_t now(::time(NULL));
  std::ostringstream query;

  query << "UPDATE mod_bam_reporting_ba_events"
           " SET end_time=" << now
        << " WHERE end_time IS NULL";
  _mysql.run_query(query.str(), "BAM-BI: could not close all ba events");

  query.str("");
  query << "UPDATE mod_bam_reporting_kpi_events"
           "  SET end_time=" << now
        << "  WHERE end_time IS NULL";
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
    int thread_id(_mysql.run_query(query, "BAM-BI: could not load timeperiods from DB"));
    mysql_result res(_mysql.get_result(thread_id));
    while (_mysql.fetch_row(thread_id, res)) {
      _timeperiods.add_timeperiod(res.value_as_u32(0),
                                  time::timeperiod::ptr(
                                            new time::timeperiod(
        res.value_as_u32(0),
        res.value_as_str(1),
        "",
        res.value_as_str(2),
        res.value_as_str(3),
        res.value_as_str(4),
        res.value_as_str(5),
        res.value_as_str(6),
        res.value_as_str(7),
        res.value_as_str(8))));
    }
  }

  // Load exceptions.
  {
    std::string query(
      "SELECT timeperiod_id, daterange, timerange"
      "  FROM mod_bam_reporting_timeperiods_exceptions");
    int thread_id(_mysql.run_query(
        query,
        "BAM-BI: could not load timeperiods exceptions from DB"));
    mysql_result res(_mysql.get_result(thread_id));
    while (_mysql.fetch_row(thread_id, res)) {
      time::timeperiod::ptr tp = _timeperiods.get_timeperiod(res.value_as_u32(0));
      if (!tp)
        logging::error(logging::high)
          << "BAM-BI: could not apply exception to non-existing timeperiod "
          << res.value_as_u32(0);
      else
        tp->add_exception(
              res.value_as_str(1),
              res.value_as_str(2));
    }
  }

  // Load exclusions.
  {
    std::string query(
      "SELECT timeperiod_id, excluded_timeperiod_id"
      "  FROM mod_bam_reporting_timeperiods_exclusions");
    int thread_id(_mysql.run_query(query, "BAM-BI: could not load exclusions from DB"));
    mysql_result res(_mysql.get_result(thread_id));
    while (_mysql.fetch_row(thread_id, res)) {
      time::timeperiod::ptr tp =
          _timeperiods.get_timeperiod(res.value_as_u32(0));
      time::timeperiod::ptr excluded_tp =
          _timeperiods.get_timeperiod(res.value_as_u32(1));
      if (!tp || !excluded_tp)
        logging::error(logging::high)
          << "BAM-BI: could not apply exclusion of timeperiod "
          << res.value_as_u32(1) << " by timeperiod "
          << res.value_as_u32(0)
          << ": at least one timeperiod does not exist";
      else
        tp->add_excluded(excluded_tp);
    }
  }

  // Load BA/timeperiods relations.
  {
    std::string query(
      "SELECT ba_id, timeperiod_id, is_default"
      "  FROM mod_bam_reporting_relations_ba_timeperiods");
    int thread_id(_mysql.run_query(
        query,
        "BAM-BI: could not load BA/timeperiods relations"));
    mysql_result res(_mysql.get_result(thread_id));
    while (_mysql.fetch_row(thread_id, res))
      _timeperiods.add_relation(
        res.value_as_u32(0),
        res.value_as_u32(1),
        res.value_as_bool(2));
  }
}

/**
 *  Prepare queries.
 */
void reporting_stream::_prepare() {
  // BA full event insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_ba_events (ba_id, "
            "            first_level, start_time, end_time, status, in_downtime)"
            "  VALUES (:ba_id, :first_level,"
            "          :start_time, :end_time, :status, :in_downtime)";
    _ba_full_event_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_ba_full_event_insert);
//    _ba_full_event_insert = _mysql.prepare(
//      query,
//      "BAM-BI: could not prepare BA full event insertion query");
  }

  // BA event update.
  {
    std::string query;
    query = "UPDATE mod_bam_reporting_ba_events"
            "  SET end_time=:end_time, first_level=:first_level,"
            "      status=:status, in_downtime=:in_downtime"
            "  WHERE ba_id=:ba_id AND start_time=:start_time";
    _ba_event_update = mysql_stmt(query, true);
    _mysql.prepare_statement(_ba_event_update);
//    _ba_event_update.prepare(
//      query,
//      "BAM-BI: could not prepare BA event update query");
  }

  // BA duration event insert.
  {
    std::string query("INSERT INTO mod_bam_reporting_ba_events_durations ("
             "                ba_event_id, start_time, "
             "                end_time, duration, sla_duration, timeperiod_id, "
             "                timeperiod_is_default)"
             "  SELECT b.ba_event_id, :start_time, :end_time, :duration, "
             "         :sla_duration, :timeperiod_id, :timeperiod_is_default"
             "  FROM mod_bam_reporting_ba_events AS b"
             "  WHERE b.ba_id=:ba_id AND b.start_time=:real_start_time");
    _ba_duration_event_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_ba_duration_event_insert);
  }

  // BA duration event update.
  {
    std::string query(
      "UPDATE mod_bam_reporting_ba_events_durations AS d"
      "  INNER JOIN mod_bam_reporting_ba_events AS e"
      "    ON d.ba_event_id=e.ba_event_id"
      "  SET d.start_time=:start_time, d.end_time=:end_time, "
      "      d.duration=:duration, d.sla_duration=:sla_duration,"
      "      d.timeperiod_is_default=:timeperiod_is_default"
      "  WHERE e.ba_id=:ba_id"
      "    AND e.start_time=:real_start_time"
      "    AND d.timeperiod_id=:timeperiod_id");
    _ba_duration_event_update = mysql_stmt(query);
    _mysql.prepare_statement(_ba_duration_event_update);
//    _ba_duration_event_update.prepare(
//      query,
//      "BAM-BI: could not prepare BA duration event update query");
  }

  // KPI full event insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_kpi_events (kpi_id,"
      "            start_time, end_time, status, in_downtime,"
      "            impact_level, first_output, first_perfdata)"
      "  VALUES (:kpi_id, :start_time, :end_time, :status,"
      "          :in_downtime, :impact_level, :output, :perfdata)");
    _kpi_full_event_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_kpi_full_event_insert);
//      query,
//      "BAM-BI: could not prepare KPI full event insertion query");
  }

  // KPI event update.
  {
    std::string query(
      "UPDATE mod_bam_reporting_kpi_events"
      "  SET end_time=:end_time, status=:status,"
      "      in_downtime=:in_downtime, impact_level=:impact_level,"
      "      first_output=:output, first_perfdata=:perfdata"
      "  WHERE kpi_id=:kpi_id AND start_time=:start_time");
    _kpi_event_update = mysql_stmt(query, true);
    _mysql.prepare_statement(_kpi_event_update);
//      query,
//      "BAM-BI: could not prepare KPI event update query");
  }

  // KPI event link to BA event.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_relations_ba_kpi_events"
      "           (ba_event_id, kpi_event_id)"
      "  SELECT be.ba_event_id, ke.kpi_event_id"
      "    FROM mod_bam_reporting_kpi_events AS ke"
      "    INNER JOIN mod_bam_reporting_ba_events AS be"
      "    ON ((ke.start_time >= be.start_time)"
      "       AND (be.end_time IS NULL OR ke.start_time < be.end_time))"
      "    INNER JOIN mod_bam_reporting_kpi AS rki"
      "     ON (rki.ba_id = be.ba_id AND rki.kpi_id = ke.kpi_id)"
      "    WHERE ke.kpi_id=:kpi_id AND ke.start_time=:start_time");
    _kpi_event_link = mysql_stmt(query, true);
    _mysql.prepare_statement(_kpi_event_link);
//      query,
//      "BAM-BI: could not prepare link query of BA and KPI events");
  }

  // Dimension BA insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
      "                sla_month_percent_crit, sla_month_percent_warn,"
      "                sla_month_duration_crit, sla_month_duration_warn)"
      " VALUES (:ba_id, :ba_name, :ba_description, :sla_month_percent_crit,"
      "         :sla_month_percent_warn, :sla_month_duration_crit,"
      "         :sla_month_duration_warn)");
    _dimension_ba_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_ba_insert);
//      query,
//      "BAM-BI: could not prepare the insertion of BA dimensions");
  }

  // Dimension BV insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_bv (bv_id, bv_name, bv_description)"
      "  VALUES (:bv_id, :bv_name, :bv_description)");
    _dimension_bv_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_bv_insert);
//      query,
//      "BAM-BI: could not prepare the insertion of BV dimensions");
  }

  // Dimension BA BV relations insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_relations_ba_bv (ba_id, bv_id)"
      "  VALUES (:ba_id, :bv_id)");
    _dimension_ba_bv_relation_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_ba_bv_relation_insert);
//      query,
//      "BAM-BI: could not prepare the insertion of BA BV relation dimension");
  }

  // Dimension timeperiod insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_timeperiods"
      "            (timeperiod_id, name, sunday, monday,"
      "             tuesday, wednesday, thursday, friday,"
      "             saturday)"
      "  VALUES (:timeperiod_id, :name, :sunday, :monday,"
      "          :tuesday, :wednesday, :thursday, :friday,"
      "          :saturday)");
    _dimension_timeperiod_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_timeperiod_insert);
//      query,
//      "BAM-BI: could not prepare timeperiod insertion query");
  }

  // Dimension timeperiod exception insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_timeperiods_exceptions"
      "            (timeperiod_id, daterange, timerange)"
      "  VALUES (:timeperiod_id, :daterange, :timerange)");
    _dimension_timeperiod_exception_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_timeperiod_exception_insert);
//      query,
//      "BAM-BI: could not prepare timeperiod exception insertion query");
  }

  // Dimension timeperiod exclusion insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_timeperiods_exclusions"
      "            (timeperiod_id, excluded_timeperiod_id)"
      "  VALUES (:timeperiod_id, :excluded_timeperiod_id)");
    _dimension_timeperiod_exclusion_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_timeperiod_exclusion_insert);
//      query,
//      "BAM-BI: could not prepare timeperiod exclusion insertion query");
  }

  // Dimension BA/timeperiod insertion.
  {
    std::string query(
      "INSERT INTO mod_bam_reporting_relations_ba_timeperiods ("
      "            ba_id, timeperiod_id, is_default)"
      "  VALUES (:ba_id, :timeperiod_id, :is_default)");
    _dimension_ba_timeperiod_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_ba_timeperiod_insert);
//      query,
//      "BAM-BI: could not prepare BA/timeperiod relation insertion query");
  }

  // Dimension truncate tables.
  {
    _dimension_truncate_tables.clear();
    std::string query("DELETE FROM mod_bam_reporting_kpi");
    _dimension_truncate_tables.push_back(mysql_stmt(query, true));
    _mysql.prepare_statement(_dimension_truncate_tables.back());
//      query,
//      "BAM-BI: could not prepare KPI deletion query");
    query = "DELETE FROM mod_bam_reporting_relations_ba_bv";
    _dimension_truncate_tables.push_back(mysql_stmt(query, true));
    _mysql.prepare_statement(_dimension_truncate_tables.back());
//      query,
//      "BAM-BI: could not prepare BA/BV relations deletion query");
    query = "DELETE FROM mod_bam_reporting_ba";
    _dimension_truncate_tables.push_back(mysql_stmt(query, true));
    _mysql.prepare_statement(_dimension_truncate_tables.back());
//      query,
//      "BAM-BI: could not prepare BA deletion query");
    query = "DELETE FROM mod_bam_reporting_bv";
    _dimension_truncate_tables.push_back(mysql_stmt(query, true));
    _mysql.prepare_statement(_dimension_truncate_tables.back());
//      query,
//      "BAM-BI: could not prepare BV deletion query");
    query = "DELETE FROM mod_bam_reporting_timeperiods";
    _dimension_truncate_tables.push_back(mysql_stmt(query, true));
    _mysql.prepare_statement(_dimension_truncate_tables.back());
//      query,
//      "BAM-BI: could not prepare timeperiods deletion query");
  }

  // Dimension KPI insertion
  {
    std::string query("INSERT INTO mod_bam_reporting_kpi (kpi_id, kpi_name,"
            "            ba_id, ba_name, host_id, host_name,"
            "            service_id, service_description, kpi_ba_id,"
            "            kpi_ba_name, meta_service_id, meta_service_name,"
            "            impact_warning, impact_critical, impact_unknown,"
            "            boolean_id, boolean_name)"
            "  VALUES (:kpi_id, :kpi_name, :ba_id, :ba_name, :host_id,"
            "          :host_name, :service_id, :service_description,"
            "          :kpi_ba_id, :kpi_ba_name, :meta_service_id,"
            "          :meta_service_name, :impact_warning, :impact_critical,"
            "          :impact_unknown, :boolean_id, :boolean_name)");
    _dimension_kpi_insert = mysql_stmt(query, true);
    _mysql.prepare_statement(_dimension_kpi_insert);
//      query,
//      "BAM-BI: could not prepare the insertion of KPIs");
  }
}

/**
 *  Process a ba event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_ba_event(misc::shared_ptr<io::data> const& e) {
  bam::ba_event const& be = e.ref_as<bam::ba_event const>();
  logging::debug(logging::low) << "BAM-BI: processing event of BA "
    << be.ba_id << " (start time " << be.start_time << ", end time "
    << be.end_time << ", status " << be.status << ", in downtime "
    << be.in_downtime << ")";

  // Try to update event.
  _ba_event_update.bind_value_as_i32(":ba_id", be.ba_id);
  _ba_event_update.bind_value_as_u64(
    ":start_time",
    static_cast<qlonglong>(be.start_time.get_time_t()));
  if (be.end_time.is_null())
    _ba_event_update.bind_value_as_null(":end_time");
  else
    _ba_event_update.bind_value_as_u64(":end_time", be.end_time.get_time_t());
  _ba_event_update.bind_value_as_tiny(":status", be.status);
  _ba_event_update.bind_value_as_bool(":in_downtime", be.in_downtime);
  _ba_event_update.bind_value_as_i32(":first_level", be.first_level);
  std::ostringstream oss_err;
  oss_err << "BAM-BI: could not update event of BA "
          << be.ba_id << " starting at " << be.start_time
          << " and ending at " << be.end_time << ": ";
  int thread_id(_mysql.run_statement(_ba_event_update, oss_err.str(), true));
  // Event was not found, insert one.
  if (_mysql.get_affected_rows(thread_id, _ba_event_update) == 0) {
    _ba_full_event_insert.bind_value_as_i32(":ba_id", be.ba_id);
    _ba_full_event_insert.bind_value_as_i32(":first_level", be.first_level);
    _ba_full_event_insert.bind_value_as_u64(
      ":start_time",
      static_cast<qlonglong>(be.start_time.get_time_t()));
    if (be.end_time.is_null())
      _ba_full_event_insert.bind_value_as_null(":end_time");
    else
      _ba_full_event_insert.bind_value_as_u64(":end_time", static_cast<qlonglong>(be.end_time.get_time_t()));
    _ba_full_event_insert.bind_value_as_tiny(":status", be.status);
    _ba_full_event_insert.bind_value_as_bool(":in_downtime", be.in_downtime);
    std::ostringstream oss_err;
    oss_err << "BAM-BI: could not insert event of BA "
            << be.ba_id << " starting at " << be.start_time
            << ": ";
    _mysql.run_statement(_ba_full_event_insert,
        oss_err.str(), true);
  }
  // Compute the associated event durations.
  if (!be.end_time.is_null() && be.start_time != be.end_time)
    _compute_event_durations(e.staticCast<bam::ba_event>(), this);
}

/**
 *  Process a ba duration event and write it to the db.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_ba_duration_event(
    misc::shared_ptr<io::data> const& e) {
  bam::ba_duration_event const& bde = e.ref_as<bam::ba_duration_event const>();
  logging::debug(logging::low) << "BAM-BI: processing BA duration event of BA "
    << bde.ba_id << " (start time " << bde.start_time << ", end time "
    << bde.end_time << ", duration " << bde.duration << ", sla duration "
    << bde.sla_duration << ")";

  // Try to update first.
  _ba_duration_event_update.bind_value_as_i32(":ba_id", bde.ba_id);
  _ba_duration_event_update.bind_value_as_u64(
    ":real_start_time",
    static_cast<qlonglong>(bde.real_start_time.get_time_t()));
  _ba_duration_event_update.bind_value_as_u64(
    ":end_time",
    static_cast<qlonglong>(bde.end_time.get_time_t()));
  _ba_duration_event_update.bind_value_as_u64(
    ":start_time",
    static_cast<qlonglong>(bde.start_time.get_time_t()));
  _ba_duration_event_update.bind_value_as_i32(":duration", bde.duration);
  _ba_duration_event_update.bind_value_as_i32(
    ":sla_duration",
    bde.sla_duration);
  _ba_duration_event_update.bind_value_as_i32(
    ":timeperiod_id",
    bde.timeperiod_id);
  _ba_duration_event_update.bind_value_as_i32(
    ":timeperiod_is_default",
    bde.timeperiod_is_default);
  int thread_id(_mysql.run_statement(_ba_duration_event_update));
  try {
    // Insert if no rows was updated.
    if (_mysql.get_affected_rows(thread_id, _ba_duration_event_update) == 0) {
      _ba_duration_event_insert.bind_value_as_i32(":ba_id", bde.ba_id);
      _ba_duration_event_insert.bind_value_as_u64(
        ":real_start_time",
        static_cast<qlonglong>(bde.real_start_time.get_time_t()));
      _ba_duration_event_insert.bind_value_as_u64(
        ":end_time",
        static_cast<qlonglong>(bde.end_time.get_time_t()));
      _ba_duration_event_insert.bind_value_as_u64(
        ":start_time",
        static_cast<qlonglong>(bde.start_time.get_time_t()));
      _ba_duration_event_insert.bind_value_as_i32(":duration", bde.duration);
      _ba_duration_event_insert.bind_value_as_i32(
        ":sla_duration",
        bde.sla_duration);
      _ba_duration_event_insert.bind_value_as_i32(
        ":timeperiod_id",
        bde.timeperiod_id);
      _ba_duration_event_insert.bind_value_as_f64(
        ":timeperiod_is_default",
        bde.timeperiod_is_default);
      _mysql.run_statement(_ba_duration_event_insert,
               "Insertion failed", true, thread_id);
    }
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: could not insert duration event of BA "
           << bde.ba_id << " starting at " << bde.start_time << ": "
           << e.what());
  }
}

/**
 *  Process a kpi event and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_kpi_event(
    misc::shared_ptr<io::data> const& e) {
  bam::kpi_event const& ke = e.ref_as<bam::kpi_event const>();
  logging::debug(logging::low) << "BAM-BI: processing event of KPI "
    << ke.kpi_id << " (start time " << ke.start_time << ", end time "
    << ke.end_time << ", state " << ke.status << ", in downtime "
    << ke.in_downtime << ")";

  // Try to update kpi.
  _kpi_event_update.bind_value_as_i32(":kpi_id", ke.kpi_id);
  _kpi_event_update.bind_value_as_u64(
    ":start_time",
    static_cast<qlonglong>(ke.start_time.get_time_t()));
  if (ke.end_time.is_null())
  _kpi_event_update.bind_value_as_null(":end_time");
  _kpi_event_update.bind_value_as_u64(
    ":end_time",
    static_cast<qlonglong>(ke.end_time.get_time_t()));
  _kpi_event_update.bind_value_as_tiny(":status", ke.status);
  _kpi_event_update.bind_value_as_i32(":in_downtime", ke.in_downtime);
  _kpi_event_update.bind_value_as_i32(":impact_level", ke.impact_level);
  _kpi_event_update.bind_value_as_str(":output", ke.output.toStdString());
  _kpi_event_update.bind_value_as_str(":perfdata", ke.perfdata.toStdString());
  std::ostringstream oss_err;
  oss_err << "BAM-BI: could not update KPI "
          << ke.kpi_id << " starting at " << ke.start_time
          << " and ending at " << ke.end_time << ": ";
  int thread_id(_mysql.run_statement(
                         _kpi_event_update,
                         oss_err.str(), true));
  // No kpis were updated, insert one.
  if (_mysql.get_affected_rows(thread_id, _kpi_event_update) == 0) {
    _kpi_full_event_insert.bind_value_as_i32(":kpi_id", ke.kpi_id);
    _kpi_full_event_insert.bind_value_as_u64(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    if (ke.end_time.is_null())
      _kpi_full_event_insert.bind_value_as_null(":end_time");
    else
      _kpi_full_event_insert.bind_value_as_u64(
        ":end_time",
        static_cast<qlonglong>(ke.end_time.get_time_t()));
    _kpi_full_event_insert.bind_value_as_tiny(":status", ke.status);
    _kpi_full_event_insert.bind_value_as_bool(":in_downtime", ke.in_downtime);
    _kpi_full_event_insert.bind_value_as_i32(":impact_level", ke.impact_level);
    _kpi_full_event_insert.bind_value_as_str(":output", ke.output.toStdString());
    _kpi_full_event_insert.bind_value_as_str(":perfdata", ke.perfdata.toStdString());
    oss_err.str("");
    oss_err << "BAM-BI: could not insert event of KPI "
            << ke.kpi_id << " starting at " << ke.start_time
            << " and ending at " << ke.end_time << ": ";
    _mysql.run_statement(
             _kpi_full_event_insert,
             oss_err.str(), true,
             thread_id);

    // Insert kpi event link.
    _kpi_event_link.bind_value_as_u64(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    _kpi_event_link.bind_value_as_i32(":kpi_id", ke.kpi_id);
    oss_err.str("");
    oss_err << "BAM-BI: could not create link from event of KPI "
            << ke.kpi_id << " starting at " << ke.start_time
            << " to its associated BA event: ";
    _mysql.run_statement(_kpi_event_link,
             oss_err.str(), true,
             thread_id);

  }
}

/**
 *  Process a dimension ba and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_ba(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_ba_event const& dba = e.ref_as<bam::dimension_ba_event const>();
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of BA "
    << dba.ba_id << " ('" << dba.ba_description << "')";
  _dimension_ba_insert.bind_value_as_i32(":ba_id", dba.ba_id);
  _dimension_ba_insert.bind_value_as_str(":ba_name", dba.ba_name.toStdString());
  _dimension_ba_insert.bind_value_as_str(
                         ":ba_description",
                         dba.ba_description.toStdString());
  _dimension_ba_insert.bind_value_as_f64(
                         ":sla_month_percent_crit",
                         dba.sla_month_percent_crit);
  _dimension_ba_insert.bind_value_as_f64(
                         ":sla_month_percent_warn",
                         dba.sla_month_percent_warn);
  _dimension_ba_insert.bind_value_as_f64(
                         ":sla_month_duration_crit",
                         dba.sla_duration_crit);
  _dimension_ba_insert.bind_value_as_f64(
                         ":sla_month_duration_warn"
                         , dba.sla_duration_warn);
  std::ostringstream oss_err;
  oss_err << "BAM-BI: could not insert BA "
          << dba.ba_id << ": ";
  _mysql.run_statement(_dimension_ba_insert, oss_err.str(), true);
}

/**
 *  Process a dimension bv and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_bv(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_bv_event const& dbv =
      e.ref_as<bam::dimension_bv_event const>();
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of BV "
    << dbv.bv_id << " ('" << dbv.bv_name << "')";
  _dimension_bv_insert.bind_value_as_i32(":bv_id", dbv.bv_id);
  _dimension_bv_insert.bind_value_as_str(":bv_name", dbv.bv_name.toStdString());
  _dimension_bv_insert.bind_value_as_str(
                         ":bv_description",
                         dbv.bv_description.toStdString());
  std::ostringstream oss;
  oss << "BAM-BI: could not insert BV " << dbv.bv_id << ": ";
  _mysql.run_statement(_dimension_bv_insert,
                         oss.str(), true);
}

/**
 *  Process a dimension ba bv relation and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_ba_bv_relation(
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_ba_bv_relation_event const& dbabv =
    e.ref_as<bam::dimension_ba_bv_relation_event const>();
  logging::debug(logging::low)
    << "BAM-BI: processing relation between BA "
    << dbabv.ba_id << " and BV " << dbabv.bv_id;
  _dimension_ba_bv_relation_insert.bind_value_as_i32(":ba_id", dbabv.ba_id);
  _dimension_ba_bv_relation_insert.bind_value_as_i32(":bv_id", dbabv.bv_id);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert dimension of BA-BV relation "
           << dbabv.ba_id << "-"<< dbabv.bv_id << ": ";
  _mysql.run_statement(_dimension_ba_bv_relation_insert,
           oss.str(), true);
}

/**
 *  Cache a dimension event, and commit it on the disk accordingly.
 *
 *  @param e  The event to process.
 */
void reporting_stream::_process_dimension(
        misc::shared_ptr<io::data> const& e) {
  // Cache the event until the end of the dimensions dump.
  _dimension_data_cache.push_back(_dimension_copy(e));

  // If this is a dimension truncate table signal, it's either the beginning
  // or the end of the dimensions dump.
  if (e->type()
      == io::events::data_type<io::events::bam,
                               bam::de_dimension_truncate_table_signal>::value) {
    dimension_truncate_table_signal const& dtts
        = e.ref_as<dimension_truncate_table_signal const>();

    if (!dtts.update_started) {
      // Lock the availability thread.
      std::unique_ptr<QMutexLocker> lock(_availabilities->lock());

      // XXX : dimension event acknowledgement might not work !!!
      //       For this reason, ignore any db error. We wouldn't
      //       be able to manage it on a stream level.
      try {
      for (std::vector<misc::shared_ptr<io::data> >::const_iterator
             it(_dimension_data_cache.begin()),
             end(_dimension_data_cache.end());
           it != end;
           ++it)
        _dimension_dispatch(*it);
      _mysql.commit();
      }
      catch (std::exception const& e) {
        logging::error(logging::medium)
          << "BAM-BI: ignored dimension insertion failure: " << e.what();
      }

      _dimension_data_cache.clear();
    }
    else
      _dimension_data_cache.erase(
        _dimension_data_cache.begin(),
        _dimension_data_cache.end() - 1);
  }
}

/**
 *  Dispatch a dimension event.
 *
 *  @param[in] data  The dimension event.
 */
void reporting_stream::_dimension_dispatch(
       misc::shared_ptr<io::data> const& data) {
  if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_ba_event>::value)
    _process_dimension_ba(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_bv_event>::value)
    _process_dimension_bv(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_ba_bv_relation_event>::value)
    _process_dimension_ba_bv_relation(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_kpi_event>::value)
    _process_dimension_kpi(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_truncate_table_signal>::value)
    _process_dimension_truncate_signal(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_timeperiod>::value)
    _process_dimension_timeperiod(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_timeperiod_exception>::value)
    _process_dimension_timeperiod_exception(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_timeperiod_exclusion>::value)
    _process_dimension_timeperiod_exclusion(data);
  else if (data->type()
           == io::events::data_type<io::events::bam,
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
misc::shared_ptr<io::data> reporting_stream::_dimension_copy(
                             misc::shared_ptr<io::data> const& data) {
  if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_ba_event>::value)
    return (new bam::dimension_ba_event(
                       data.ref_as<bam::dimension_ba_event>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_bv_event>::value)
    return (new bam::dimension_bv_event(
                       data.ref_as<bam::dimension_bv_event>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_ba_bv_relation_event>::value)
    return (new bam::dimension_ba_bv_relation_event(
                       data.ref_as<bam::dimension_ba_bv_relation_event>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_kpi_event>::value)
    return (new bam::dimension_kpi_event(
                       data.ref_as<bam::dimension_kpi_event>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_truncate_table_signal>::value)
    return (new bam::dimension_truncate_table_signal(
                       data.ref_as<bam::dimension_truncate_table_signal>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_timeperiod>::value)
    return (new bam::dimension_timeperiod(
                       data.ref_as<bam::dimension_timeperiod>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_timeperiod_exception>::value)
    return (new bam::dimension_timeperiod_exception(
                       data.ref_as<bam::dimension_timeperiod_exception>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_timeperiod_exclusion>::value)
    return (new bam::dimension_timeperiod_exclusion(
                       data.ref_as<bam::dimension_timeperiod_exclusion>()));
  else if (data->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_dimension_ba_timeperiod_relation>::value)
    return (new bam::dimension_ba_timeperiod_relation(
                       data.ref_as<bam::dimension_ba_timeperiod_relation>()));
  return (misc::shared_ptr<io::data>());
}

/**
 *  Process a dimension truncate signal and write it to the db.
 *
 *  @param[in] e The event.
 */
void reporting_stream::_process_dimension_truncate_signal(
    misc::shared_ptr<io::data> const& e) {
  dimension_truncate_table_signal const& dtts
      = e.ref_as<dimension_truncate_table_signal const>();

  if (dtts.update_started) {
    logging::debug(logging::low)
      << "BAM-BI: processing table truncation signal";

    for (std::vector<mysql_stmt>::iterator
           it(_dimension_truncate_tables.begin()),
           end(_dimension_truncate_tables.end());
         it != end;
         ++it)
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
    misc::shared_ptr<io::data> const& e) {
  bam::dimension_kpi_event const& dk =
      e.ref_as<bam::dimension_kpi_event const>();
  QString kpi_name;
  if (!dk.service_description.isEmpty())
    kpi_name = dk.host_name + " " + dk.service_description;
  else if (!dk.kpi_ba_name.isEmpty())
    kpi_name = dk.kpi_ba_name;
  else if (!dk.boolean_name.isEmpty())
    kpi_name = dk.boolean_name;
  else if (!dk.meta_service_name.isEmpty())
    kpi_name = dk.meta_service_name;
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of KPI "
    << dk.kpi_id << " ('" << kpi_name << "')";
  _dimension_kpi_insert.bind_value_as_i32(":kpi_id", dk.kpi_id);
  _dimension_kpi_insert.bind_value_as_str(":kpi_name", kpi_name.toStdString());
  _dimension_kpi_insert.bind_value_as_i32(":ba_id", dk.ba_id);
  _dimension_kpi_insert.bind_value_as_str(":ba_name", dk.ba_name.toStdString());
  _dimension_kpi_insert.bind_value_as_i32(":host_id", dk.host_id);
  _dimension_kpi_insert.bind_value_as_str(":host_name", dk.host_name.toStdString());
  _dimension_kpi_insert.bind_value_as_i32(":service_id", dk.service_id);
  _dimension_kpi_insert.bind_value_as_str(
                          ":service_description",
                          dk.service_description.toStdString());
  if (dk.kpi_ba_id)
    _dimension_kpi_insert.bind_value_as_i32(
                            ":kpi_ba_id",
                             dk.kpi_ba_id);
  else
    _dimension_kpi_insert.bind_value_as_null(":kpi_ba_id");
  _dimension_kpi_insert.bind_value_as_str(":kpi_ba_name",
                          dk.kpi_ba_name.toStdString());
  _dimension_kpi_insert.bind_value_as_i32(
                          ":meta_service_id",
                          dk.meta_service_id);
  _dimension_kpi_insert.bind_value_as_str(
                          ":meta_service_name",
                          dk.meta_service_name.toStdString());
  _dimension_kpi_insert.bind_value_as_f64(
                          ":impact_warning",
                          dk.impact_warning);
  _dimension_kpi_insert.bind_value_as_f64(
                          ":impact_critical",
                          dk.impact_critical);
  _dimension_kpi_insert.bind_value_as_f64(
                          ":impact_unknown",
                          dk.impact_unknown);
  _dimension_kpi_insert.bind_value_as_i32(":boolean_id", dk.boolean_id);
  _dimension_kpi_insert.bind_value_as_str(
                          ":boolean_name",
                          dk.boolean_name.toStdString());
  std::ostringstream oss;
  oss << "BAM-BI: could not insert dimension of KPI "
      << dk.kpi_id << ": ";
  _mysql.run_statement(
           _dimension_kpi_insert,
           oss.str(), true);
}

/**
 *  Process a dimension timeperiod and store it in the DB and in the
 *  timeperiod cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod(
                         misc::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod const& tp =
      e.ref_as<bam::dimension_timeperiod const>();
  logging::debug(logging::low)
    << "BAM-BI: processing declaration of timeperiod "
    << tp.id << " ('" << tp.name << "')";
  _dimension_timeperiod_insert.bind_value_as_i32(":timeperiod_id", tp.id);
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":name",
                                 tp.name.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":sunday",
                                 tp.sunday.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":monday",
                                 tp.monday.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":tuesday",
                                 tp.tuesday.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":wednesday",
                                 tp.wednesday.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":thursday",
                                 tp.thursday.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":friday",
                                 tp.friday.toStdString());
  _dimension_timeperiod_insert.bind_value_as_str(
                                 ":saturday",
                                 tp.saturday.toStdString());
  std::ostringstream oss;
  oss << "BAM-BI: could not insert timeperiod "
      << tp.id << " ('" << tp.name.toStdString() << "'): ";
  _mysql.run_statement(_dimension_timeperiod_insert,
           oss.str(), true);
  _apply(tp);
}

/**
 *  Process a timeperiod exception and store it in the DB and in the
 *  timeperiod cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod_exception(
                         misc::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod_exception const& tpe =
    e.ref_as<bam::dimension_timeperiod_exception const>();
  logging::debug(logging::low)
    << "BAM-BI: processing exception of timeperiod " << tpe.timeperiod_id;
  _dimension_timeperiod_exception_insert.bind_value_as_i32(
                                           ":timeperiod_id",
                                           tpe.timeperiod_id);
  _dimension_timeperiod_exception_insert.bind_value_as_str(
                                           ":daterange",
                                           tpe.daterange.toStdString());
  _dimension_timeperiod_exception_insert.bind_value_as_str(
                                           ":timerange",
                                           tpe.timerange.toStdString());
  std::ostringstream oss;
  oss << "BAM-BI: could not insert exception of timeperiod "
      << tpe.timeperiod_id << ": ";
  _mysql.run_statement(
           _dimension_timeperiod_exception_insert,
           oss.str(), true);
  _apply(tpe);
}

/**
 *  Process a timeperiod exclusion and store it in the DB and in the
 *  timeperiod cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_timeperiod_exclusion(
                         misc::shared_ptr<io::data> const& e) {
  bam::dimension_timeperiod_exclusion const& tpe =
    e.ref_as<bam::dimension_timeperiod_exclusion const>();
  logging::debug(logging::low)
    << "BAM-BI: processing exclusion of timeperiod "
    << tpe.excluded_timeperiod_id << " by timeperiod "
    << tpe.timeperiod_id;
  _dimension_timeperiod_exclusion_insert.bind_value_as_i32(
                                           ":timeperiod_id",
                                           tpe.timeperiod_id);
  _dimension_timeperiod_exclusion_insert.bind_value_as_i32(
                                           ":excluded_timeperiod_id",
                                           tpe.excluded_timeperiod_id);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert exclusion of timeperiod "
      << tpe.excluded_timeperiod_id << " by timeperiod "
      << tpe.timeperiod_id << ": ";
  _mysql.run_statement(
           _dimension_timeperiod_exclusion_insert,
           oss.str(), true);
  _apply(tpe);
}

/**
 *  Process a dimension ba timeperiod relation and store it in
 *  a relation cache.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_dimension_ba_timeperiod_relation(
        misc::shared_ptr<io::data> const& e) {
  bam::dimension_ba_timeperiod_relation const& r =
     e.ref_as<bam::dimension_ba_timeperiod_relation const>();
  logging::debug(logging::low)
    << "BAM-BI: processing relation of BA " << r.ba_id
    << " to timeperiod " << r.timeperiod_id;
  _dimension_ba_timeperiod_insert.bind_value_as_i32(":ba_id", r.ba_id);
  _dimension_ba_timeperiod_insert.bind_value_as_i32(
                                    ":timeperiod_id",
                                    r.timeperiod_id);
  _dimension_ba_timeperiod_insert.bind_value_as_bool(
                                    ":is_default",
                                    r.is_default);
  std::ostringstream oss;
  oss << "BAM-BI: could not insert relation of BA "
      << r.ba_id << " to timeperiod " << r.timeperiod_id << ": ";
  _mysql.run_statement(
           _dimension_ba_timeperiod_insert,
           oss.str(), true);
  _timeperiods.add_relation(
                 r.ba_id,
                 r.timeperiod_id,
                 r.is_default);
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
                         misc::shared_ptr<ba_event> const& ev,
                         io::stream* visitor) {
  if (ev.isNull() || !visitor)
    return ;

  logging::info(logging::medium)
    << "BAM-BI: computing durations of event started at "
    << ev->start_time << " and ended at " << ev->end_time
    << " on BA " << ev->ba_id;

  // Find the timeperiods associated with this ba.
  std::vector<std::pair<time::timeperiod::ptr, bool> >
    timeperiods = _timeperiods.get_timeperiods_by_ba_id(ev->ba_id);

  if (timeperiods.empty()) {
    logging::debug(logging::medium)
      << "BAM-BI: no reporting period defined for event started at "
      << ev->start_time << " and ended at " << ev->end_time
      << " on BA " << ev->ba_id;
    return ;
  }

  for (std::vector<std::pair<time::timeperiod::ptr, bool> >::const_iterator
         it(timeperiods.begin()),
         end(timeperiods.end());
       it != end;
       ++it) {
    time::timeperiod::ptr tp = it->first;
    bool is_default = it->second;

    misc::shared_ptr<ba_duration_event> dur_ev(new ba_duration_event);
    dur_ev->ba_id = ev->ba_id;
    dur_ev->real_start_time = ev->start_time;
    dur_ev->start_time = tp->get_next_valid(ev->start_time);
    dur_ev->end_time = ev->end_time;
    if ((dur_ev->start_time != (time_t)-1)
        && (dur_ev->end_time != (time_t)-1)
        && (dur_ev->start_time < dur_ev->end_time)) {
      dur_ev->duration = dur_ev->end_time - dur_ev->start_time;
      dur_ev->sla_duration = tp->duration_intersect(
                                   dur_ev->start_time,
                                   dur_ev->end_time);
      dur_ev->timeperiod_id = tp->get_id();
      dur_ev->timeperiod_is_default = is_default;
      logging::debug(logging::low)
        << "BAM-BI: durations of event started at " << ev->start_time
        << " and ended at " << ev->end_time << " on BA " << ev->ba_id
        << " were computed for timeperiod " << tp->get_name()
        << ", duration is " << dur_ev->duration << "s, SLA duration is "
        << dur_ev->sla_duration;
      visitor->write(dur_ev.staticCast<io::data>());
    }
    else
      logging::debug(logging::medium)
        << "BAM-BI: event started at " << ev->start_time
        << " and ended at " << ev->end_time << " on BA " << ev->ba_id
        << " has no duration on timeperiod " << tp->get_name();
  }
}

/**
 *  Process a rebuild signal: Delete the obsolete data in the db and rebuild
 *  ba duration events.
 *
 *  @param[in] e  The event.
 */
void reporting_stream::_process_rebuild(misc::shared_ptr<io::data> const& e) {
  rebuild const& r = e.ref_as<rebuild const>();
  if (r.bas_to_rebuild.isEmpty())
    return ;
  logging::debug(logging::low)
    << "BAM-BI: processing rebuild signal";

  _update_status("rebuilding: querying ba events");

  // We block the availability thread to prevent it waking
  // up on truncated event durations.
  try {
    std::unique_ptr<QMutexLocker> lock(_availabilities->lock());

    // Delete obsolete ba events durations.
    {
      std::string query(
        "DELETE a"
        "  FROM mod_bam_reporting_ba_events_durations as a"
        "    INNER JOIN mod_bam_reporting_ba_events as b"
        "      ON a.ba_event_id = b.ba_event_id"
        "  WHERE b.ba_id IN (");
      query.append(r.bas_to_rebuild.toStdString());
      query.append(")");
      std::ostringstream oss;
      oss << "BAM-BI: could not delete BA durations "
          << r.bas_to_rebuild.toStdString() << ": ";
      _mysql.run_query(query, oss.str(), true);
    }

    // Get the ba events.
    std::vector<misc::shared_ptr<ba_event> > ba_events;
    {
      std::string query(
              "SELECT ba_id, start_time, end_time, "
              "       status, in_downtime boolean"
              "  FROM mod_bam_reporting_ba_events"
              "  WHERE end_time IS NOT NULL"
              "    AND ba_id IN (");
      query.append(r.bas_to_rebuild.toStdString());
      query.append(")");
      std::ostringstream oss;
      oss << "BAM-BI: could not get BA events of "
          << r.bas_to_rebuild.toStdString() << " :";
      int thread_id(_mysql.run_query(query, oss.str(), true));
      mysql_result res(_mysql.get_result(thread_id));

      while (_mysql.fetch_row(thread_id, res)) {
        misc::shared_ptr<ba_event> baev(new ba_event);
        baev->ba_id = res.value_as_i32(0);
        baev->start_time = res.value_as_i32(1);
        baev->end_time = res.value_as_i32(2);
        baev->status = res.value_as_i32(3);
        baev->in_downtime = res.value_as_bool(4);
        ba_events.push_back(baev);
        logging::debug(logging::low)
          << "BAM-BI: got events of BA " << baev->ba_id;
      }
    }

    logging::info(logging::medium)
      << "BAM-BI: will now rebuild the event durations";

    size_t ba_events_num = ba_events.size();
    size_t ba_events_curr = 0;
    std::stringstream ss;

    // Generate new ba events durations for each ba events.
    {
      for (std::vector<misc::shared_ptr<ba_event> >::const_iterator
             it(ba_events.begin()),
             end(ba_events.end());
          it != end;
          ++it, ++ba_events_curr) {
        ss.str("");
        ss << "rebuilding: ba event " << ba_events_curr
           << "/" << ba_events_num;
        _update_status(ss.str());
        _compute_event_durations(*it, this);
      }
    }
  } catch(...) {
    _update_status("");
    throw ;
  }

  logging::info(logging::medium)
    << "BAM-BI: event durations rebuild finished, "
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
  QMutexLocker lock(&_statusm);
  _status = status;
  return ;
}
