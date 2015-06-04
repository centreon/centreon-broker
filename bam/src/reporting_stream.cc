/*
** Copyright 2014-2015 Merethis
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

#include <cassert>
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
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
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
    _db(db_cfg),
    _ba_event_insert(_db),
    _ba_full_event_insert(_db),
    _ba_event_update(_db),
    _ba_event_delete(_db),
    _ba_duration_event_insert(_db),
    _kpi_event_insert(_db),
    _kpi_full_event_insert(_db),
    _kpi_event_update(_db),
    _kpi_event_delete(_db),
    _kpi_event_link(_db),
    _dimension_ba_insert(_db),
    _dimension_bv_insert(_db),
    _dimension_ba_bv_relation_insert(_db),
    _dimension_timeperiod_insert(_db),
    _dimension_timeperiod_exception_insert(_db),
    _dimension_timeperiod_exclusion_insert(_db),
    _dimension_ba_timeperiod_insert(_db),
    _dimension_kpi_insert(_db) {
  // Prepare queries.
  _prepare();

  // Load timeperiods.
  _load_timeperiods();

  // Load last events stored in DB.
  _load_last_events();

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
  throw (io::exceptions::shutdown(true, false)
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
  if (!_status.empty()) {
    io::property& p(tree["status"]);
    p.set_perfdata(_status);
    p.set_graphable(false);
  }
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int reporting_stream::write(misc::shared_ptr<io::data> const& data) {
  if (!data.isNull()) {
    ++_pending_events;
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
  }
  else
    _db.commit();

  // Event acknowledgement.
  if (!_db.pending_queries()) {
    int retval(_pending_events);
    _pending_events = 0;
    return (retval);
  }
  else
    return (0);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] other Unused.
 */
reporting_stream::reporting_stream(reporting_stream const& other)
  : io::stream(other),
    _db(database_config("", "", 0, "", "", "")),
    _ba_event_insert(_db),
    _ba_full_event_insert(_db),
    _ba_event_update(_db),
    _ba_event_delete(_db),
    _ba_duration_event_insert(_db),
    _kpi_event_insert(_db),
    _kpi_full_event_insert(_db),
    _kpi_event_update(_db),
    _kpi_event_delete(_db),
    _kpi_event_link(_db),
    _dimension_ba_insert(_db),
    _dimension_bv_insert(_db),
    _dimension_ba_bv_relation_insert(_db),
    _dimension_timeperiod_insert(_db),
    _dimension_timeperiod_exception_insert(_db),
    _dimension_timeperiod_exclusion_insert(_db),
    _dimension_ba_timeperiod_insert(_db),
    _dimension_kpi_insert(_db) {
  assert(!"BAM reporting stream is not copyable");
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] other Unused.
 *
 *  @return This object.
 */
reporting_stream& reporting_stream::operator=(reporting_stream const& other) {
  (void)other;
  assert(!"BAM reporting stream is not copyable");
  abort();
  return (*this);
}

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
  database_query q(_db);

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
    q.run_query(
        query.str(),
        "BAM-BI: could not get inconsistent events");
    while (q.next())
      events.push_back(std::make_pair(
               q.value(0).toUInt(),
               static_cast<time_t>(q.value(1).toLongLong())));
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
      try {
        q.run_query(oss.str());
        if (!q.next())
          throw (exceptions::msg() << "no event following this one");
      }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not get end time of inconsistent event of "
               << event_type << " " << it->first << " starting at "
               << it->second << ": " << e.what());
      }
      end_time = q.value(0).toLongLong();
    }
    {
      std::ostringstream oss;
      oss << "UPDATE " << table
          << "  SET end_time=" << end_time
          << "  WHERE " << id << "=" << it->first
          << "  AND start_time=" << it->second;
      try { q.run_query(oss.str()); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not close inconsistent event of "
               << event_type << it->first << " starting at "
               << it->second << ": " << e.what());
      }
    }
  }
  return ;
}

/**
 *  Load last BA/KPI events from DB.
 */
void reporting_stream::_load_last_events() {
  // Get the BA list.
  std::list<unsigned int> ids;
  {
    std::string query("SELECT ba_id FROM mod_bam_reporting_ba");
    database_query q(_db);
    q.run_query(
        query,
        "BAM-BI: could not fetch the list of existing BAs");
    while (q.next())
      ids.push_back(q.value(0).toUInt());
  }

  // Delete inconsistent entries.
  {
    _close_inconsistent_events(
      "BA",
      "mod_bam_reporting_ba_events",
      "ba_id");
    _close_inconsistent_events(
      "KPI",
      "mod_bam_reporting_kpi_events",
      "kpi_id");
  }

  // Load the last two events for each BA.
  for (std::list<unsigned int>::const_iterator
         it(ids.begin()),
         end(ids.end());
       it != end;
       ++it) {
    std::ostringstream oss;
    oss << "SELECT start_time, end_time, first_level,"
        << "       status, in_downtime"
        << "  FROM mod_bam_reporting_ba_events"
        << "  WHERE ba_id=" << *it
        << "  ORDER BY start_time DESC"
        << "  LIMIT 2";
    database_query q(_db);
    try { q.run_query(oss.str()); }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM-BI: could not fetch last events of BA "
             << *it << ": " << e.what());
    }
    while (q.next()) {
      ba_event bae;
      bae.ba_id = *it;
      bae.start_time = q.value(0).toLongLong();
      bae.end_time = (q.value(1).isNull()
                      ? (time_t)-1
                      : (time_t)q.value(1).toLongLong());
      bae.first_level = q.value(2).toDouble();
      bae.status = q.value(3).toInt();
      bae.in_downtime = q.value(4).toBool();
      _ba_event_cache[*it].push_back(bae);
    }
  }

  // Get the KPI list.
  ids.clear();
  {
    std::string query("SELECT kpi_id FROM mod_bam_reporting_kpi");
    database_query q(_db);
    q.run_query(
        query,
        "BAM-BI: could not fetch the list of existing KPI");
    while (q.next())
      ids.push_back(q.value(0).toUInt());
  }

  // Load the last two events for each KPI.
  for (std::list<unsigned int>::const_iterator
         it(ids.begin()),
         end(ids.end());
       it != end;
       ++it) {
    std::ostringstream oss;
    oss << "SELECT start_time, end_time, status, in_downtime,"
        << "       impact_level, first_output, first_perfdata"
        << "  FROM mod_bam_reporting_kpi_events"
        << "  WHERE kpi_id=" << *it
        << "  ORDER BY start_time DESC"
        << "  LIMIT 2";
    database_query q(_db);
    try { q.run_query(oss.str()); }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM-BI: could not fetch last events of KPI "
             << *it << ": " << e.what());
    }
    while (q.next()) {
      kpi_event kpie;
      kpie.kpi_id = *it;
      kpie.start_time = q.value(0).toLongLong();
      kpie.end_time = (q.value(1).isNull()
                       ? (time_t)-1
                       : (time_t)q.value(1).toLongLong());
      kpie.status = q.value(2).toInt();
      kpie.in_downtime = q.value(3).toBool();
      kpie.impact_level = q.value(4).toInt();
      kpie.output = q.value(5).toString();
      kpie.perfdata = q.value(6).toString();
      _kpi_event_cache[*it].push_back(kpie);
    }
  }

  return ;
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
    database_query q(_db);
    q.run_query(query, "BAM-BI: could not load timeperiods from DB");
    while (q.next()) {
      _timeperiods.add_timeperiod(q.value(0).toUInt(),
                                  time::timeperiod::ptr(
                                            new time::timeperiod(
        q.value(0).toUInt(),
        q.value(1).toString().toStdString(),
        "",
        q.value(2).toString().toStdString(),
        q.value(3).toString().toStdString(),
        q.value(4).toString().toStdString(),
        q.value(5).toString().toStdString(),
        q.value(6).toString().toStdString(),
        q.value(7).toString().toStdString(),
        q.value(8).toString().toStdString())));
    }
  }

  // Load exceptions.
  {
    std::string query(
      "SELECT timeperiod_id, daterange, timerange"
      "  FROM mod_bam_reporting_timeperiods_exceptions");
    database_query q(_db);
    q.run_query(
        query,
        "BAM-BI: could not load timeperiods exceptions from DB");
    while (q.next()) {
      time::timeperiod::ptr tp = _timeperiods.get_timeperiod(q.value(0).toUInt());
      if (!tp)
        logging::error(logging::high)
          << "BAM-BI: could not apply exception to non-existing timeperiod "
          << q.value(0).toUInt();
      else
        tp->add_exception(
                      q.value(1).toString().toStdString(),
                      q.value(2).toString().toStdString());
    }
  }

  // Load exclusions.
  {
    std::string query(
      "SELECT timeperiod_id, excluded_timeperiod_id"
      "  FROM mod_bam_reporting_timeperiods_exclusions");
    database_query q(_db);
    q.run_query(query, "BAM-BI: could not load exclusions from DB");
    while (q.next()) {
      time::timeperiod::ptr tp =
          _timeperiods.get_timeperiod(q.value(0).toUInt());
      time::timeperiod::ptr excluded_tp =
          _timeperiods.get_timeperiod(q.value(1).toUInt());
      if (!tp || !excluded_tp)
        logging::error(logging::high)
          << "BAM-BI: could not apply exclusion of timeperiod "
          << q.value(1).toUInt() << " by timeperiod "
          << q.value(0).toUInt()
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
    database_query q(_db);
    q.run_query(
        query,
        "BAM-BI: could not load BA/timeperiods relations");
    while (q.next())
      _timeperiods.add_relation(
        q.value(0).toUInt(),
        q.value(1).toUInt(),
        q.value(2).toBool());
  }

  return ;
}

/**
 *  Prepare queries.
 */
void reporting_stream::_prepare() {
  // BA event insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_ba_events (ba_id, "
            "            first_level, start_time, status, in_downtime)"
            "  VALUES (:ba_id, :first_level,"
            "          :start_time, :status, :in_downtime)";
    _ba_event_insert.prepare(
      query,
      "BAM-BI: could not prepare BA event insertion query");
  }

  // BA full event insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_ba_events (ba_id, "
            "            first_level, start_time, end_time, status, in_downtime)"
            "  VALUES (:ba_id, :first_level,"
            "          :start_time, :end_time, :status, :in_downtime)";
    _ba_full_event_insert.prepare(
      query,
      "BAM-BI: could not prepare BA full event insertion query");
  }

  // BA event update.
  {
    std::string query;
    query = "UPDATE mod_bam_reporting_ba_events"
            "  SET end_time=:end_time"
            "  WHERE ba_id=:ba_id AND start_time=:start_time";
    _ba_event_update.prepare(
      query,
      "BAM-BI: could not prepare BA event update query");
  }

  // BA event deletion.
  {
    std::string query;
    query = "DELETE FROM mod_bam_reporting_ba_events"
            "  WHERE ba_id=:ba_id AND start_time=:start_time";
    _ba_event_delete.prepare(
      query,
      "BAM-BI: could not prepare BA event deletion query");
  }

  // BA duration event insert.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_ba_events_durations ("
             "                ba_event_id, start_time, "
             "                end_time, duration, sla_duration, timeperiod_id, "
             "                timeperiod_is_default)"
             "  SELECT b.ba_event_id, :start_time, :end_time, :duration, "
             "         :sla_duration, :timeperiod_id, :timeperiod_is_default"
             "  FROM mod_bam_reporting_ba_events AS b"
             "  WHERE b.ba_id=:ba_id AND b.start_time=:real_start_time";
    _ba_duration_event_insert.prepare(
      query,
      "BAM-BI: could not prepare BA duration event insert query");
  }

  // KPI event insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_kpi_events (kpi_id,"
            "            start_time, status, in_downtime, impact_level,"
            "            first_output, first_perfdata)"
            "  VALUES (:kpi_id, :start_time, :status, :in_downtime, "
            "         :impact_level, :output, :perfdata)";
    _kpi_event_insert.prepare(
      query,
      "BAM-BI: could not prepare KPI event insertion query");
  }

  // KPI full event insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_kpi_events (kpi_id,"
            "            start_time, end_time, status, in_downtime,"
            "            impact_level, first_output, first_perfdata)"
            "  VALUES (:kpi_id, :start_time, :end_time, :status,"
            "          :in_downtime, :impact_level, :output, :perfdata)";
    _kpi_full_event_insert.prepare(
      query,
      "BAM-BI: could not prepare KPI full event insertion query");
  }

  // KPI event update.
  {
    std::string query;
    query = "UPDATE mod_bam_reporting_kpi_events"
            "  SET end_time=:end_time"
            "  WHERE kpi_id=:kpi_id AND start_time=:start_time";
    _kpi_event_update.prepare(
      query,
      "BAM-BI: could not prepare KPI event update query");
  }

  // KPI event deletion.
  {
    std::string query;
    query = "DELETE FROM mod_bam_reporting_kpi_events"
            "  WHERE kpi_id=:kpi_id AND start_time=:start_time";
    _kpi_event_delete.prepare(
      query,
      "BAM-BI: could not prepare KPI event deletion query");
  }

  // KPI event link to BA event.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_relations_ba_kpi_events"
            "           (ba_event_id, kpi_event_id)"
            "  SELECT be.ba_event_id, ke.kpi_event_id"
            "    FROM mod_bam_reporting_kpi_events AS ke"
            "    INNER JOIN mod_bam_reporting_ba_events AS be"
            "    ON ((ke.start_time >= be.start_time)"
            "       AND (be.end_time IS NULL OR ke.start_time < be.end_time))"
            "    INNER JOIN mod_bam_reporting_kpi AS rki"
            "     ON (rki.ba_id = be.ba_id AND rki.kpi_id = ke.kpi_id)"
            "    WHERE ke.kpi_id=:kpi_id AND ke.start_time=:start_time";
    _kpi_event_link.prepare(
      query,
      "BAM-BI: could not prepare link query of BA and KPI events");
  }

  // Dimension BA insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_ba (ba_id, ba_name, ba_description,"
            "                sla_month_percent_crit, sla_month_percent_warn,"
            "                sla_month_duration_crit, sla_month_duration_warn)"
            " VALUES (:ba_id, :ba_name, :ba_description, :sla_month_percent_crit,"
            "         :sla_month_percent_warn, :sla_month_duration_crit,"
            "         :sla_month_duration_warn)";
    _dimension_ba_insert.prepare(
      query,
      "BAM-BI: could not prepare the insertion of BA dimensions");
  }

  // Dimension BV insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_bv (bv_id, bv_name, bv_description)"
            "  VALUES (:bv_id, :bv_name, :bv_description)";
    _dimension_bv_insert.prepare(
      query,
      "BAM-BI: could not prepare the insertion of BV dimensions");
  }

  // Dimension BA BV relations insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_relations_ba_bv (ba_id, bv_id)"
            "  VALUES (:ba_id, :bv_id)";
    _dimension_ba_bv_relation_insert.prepare(
      query,
      "BAM-BI: could not prepare the insertion of BA BV relation dimension");
  }

  // Dimension timeperiod insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_timeperiods"
            "            (timeperiod_id, name, sunday, monday,"
            "             tuesday, wednesday, thursday, friday,"
            "             saturday)"
            "  VALUES (:timeperiod_id, :name, :sunday, :monday,"
            "          :tuesday, :wednesday, :thursday, :friday,"
            "          :saturday)";
    _dimension_timeperiod_insert.prepare(
      query,
      "BAM-BI: could not prepare timeperiod insertion query");
  }

  // Dimension timeperiod exception insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_timeperiods_exceptions"
            "            (timeperiod_id, daterange, timerange)"
            "  VALUES (:timeperiod_id, :daterange, :timerange)";
    _dimension_timeperiod_exception_insert.prepare(
      query,
      "BAM-BI: could not prepare timeperiod exception insertion query");
  }

  // Dimension timeperiod exclusion insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_timeperiods_exclusions"
            "            (timeperiod_id, excluded_timeperiod_id)"
            "  VALUES (:timeperiod_id, :excluded_timeperiod_id)";
    _dimension_timeperiod_exclusion_insert.prepare(
      query,
      "BAM-BI: could not prepare timeperiod exclusion insertion query");
  }

  // Dimension BA/timeperiod insertion.
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_relations_ba_timeperiods ("
            "            ba_id, timeperiod_id, is_default)"
            "  VALUES (:ba_id, :timeperiod_id, :is_default)";
    _dimension_ba_timeperiod_insert.prepare(
      query,
      "BAM-BI: could not prepare BA/timeperiod relation insertion query");
  }

  // Dimension truncate tables.
  {
    _dimension_truncate_tables.clear();
    std::string query;
    query = "DELETE FROM mod_bam_reporting_kpi";
    _dimension_truncate_tables.push_back(
      misc::shared_ptr<database_query>(new database_query(_db)));
    _dimension_truncate_tables.back()->prepare(
      query,
      "BAM-BI: could not prepare KPI deletion query");
    query = "DELETE FROM mod_bam_reporting_relations_ba_bv";
    _dimension_truncate_tables.push_back(
      misc::shared_ptr<database_query>(new database_query(_db)));
    _dimension_truncate_tables.back()->prepare(
      query,
      "BAM-BI: could not prepare BA/BV relations deletion query");
    query = "DELETE FROM mod_bam_reporting_ba";
    _dimension_truncate_tables.push_back(
      misc::shared_ptr<database_query>(new database_query(_db)));
    _dimension_truncate_tables.back()->prepare(
      query,
      "BAM-BI: could not prepare BA deletion query");
    query = "DELETE FROM mod_bam_reporting_bv";
    _dimension_truncate_tables.push_back(
      misc::shared_ptr<database_query>(new database_query(_db)));
    _dimension_truncate_tables.back()->prepare(
      query,
      "BAM-BI: could not prepare BV deletion query");
    query = "DELETE FROM mod_bam_reporting_timeperiods";
    _dimension_truncate_tables.push_back(
      misc::shared_ptr<database_query>(new database_query(_db)));
    _dimension_truncate_tables.back()->prepare(
      query,
      "BAM-BI: could not prepare timeperiods deletion query");
  }

  // Dimension KPI insertion
  {
    std::string query;
    query = "INSERT INTO mod_bam_reporting_kpi (kpi_id, kpi_name,"
            "            ba_id, ba_name, host_id, host_name,"
            "            service_id, service_description, kpi_ba_id,"
            "            kpi_ba_name, meta_service_id, meta_service_name,"
            "            impact_warning, impact_critical, impact_unknown,"
            "            boolean_id, boolean_name)"
            "  VALUES (:kpi_id, :kpi_name, :ba_id, :ba_name, :host_id,"
            "          :host_name, :service_id, :service_description,"
            "          :kpi_ba_id, :kpi_ba_name, :meta_service_id,"
            "          :meta_service_name, :impact_warning, :impact_critical,"
            "          :impact_unknown, :boolean_id, :boolean_name)";
    _dimension_kpi_insert.prepare(
      query,
      "BAM-BI: could not prepare the insertion of KPIs");
  }

  return ;
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
  // Ephemeral event.
  if (be.start_time == be.end_time) {
    _ba_event_delete.bind_value(":ba_id", be.ba_id);
    _ba_event_delete.bind_value(
      ":start_time",
      static_cast<qlonglong>(be.start_time.get_time_t()));
    try { _ba_event_delete.run_statement(); }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM-BI: could not delete ephemeral event of BA "
             << be.ba_id << " at second " << be.start_time
             << ": " << e.what());
    }
    std::map<unsigned int, std::list<ba_event> >::iterator
      it(_ba_event_cache.find(be.ba_id));
    if ((it != _ba_event_cache.end())
        && !it->second.empty()
        && (be.start_time == it->second.front().start_time)) {
      it->second.pop_front();
      if (it->second.empty())
        _ba_event_cache.erase(it);
    }
  }
  // End of event.
  else if ((be.end_time != 0) && (be.end_time != (time_t)-1)) {
    std::map<unsigned int, std::list<ba_event> >::iterator
      it(_ba_event_cache.find(be.ba_id));
    if (it == _ba_event_cache.end()) {
      _ba_event_cache[be.ba_id].push_front(be);
      it = _ba_event_cache.find(be.ba_id);
    }
    else if (it->second.empty())
      it->second.push_front(be);
    else
      it->second.front().end_time = be.end_time;
    _ba_event_update.bind_value(":ba_id", be.ba_id);
    _ba_event_update.bind_value(
      ":start_time",
      static_cast<qlonglong>(it->second.front().start_time.get_time_t()));
    _ba_event_update.bind_value(
      ":end_time",
      static_cast<qlonglong>(it->second.front().end_time.get_time_t()));
    try { _ba_event_update.run_statement(); }
    catch (std::exception const& e) {
      throw (exceptions::msg() << "BAM-BI: could not close event of BA "
             << be.ba_id << " starting at " << be.start_time
             << " and ending at " << be.end_time << ": " << e.what());
    }

    // If nothing was updated (can happen when there is a gap between
    // the events for some reasons, then insert a new event in the database.
    if (_ba_event_update.num_rows_affected() == 0) {
      logging::error(logging::medium)
        << "BAM-BI: could not update the event of the BA " << be.ba_id
        << " starting at " << it->second.front().start_time.get_time_t()
        << " and ending at " << it->second.front().end_time.get_time_t()
        << ": inserting a new event instead";
      _ba_full_event_insert.bind_value(":ba_id", be.ba_id);
      _ba_full_event_insert.bind_value(":first_level", be.first_level);
      _ba_full_event_insert.bind_value(
        ":start_time",
        static_cast<qlonglong>(be.start_time.get_time_t()));
      _ba_full_event_insert.bind_value(
        ":end_time",
        static_cast<qlonglong>(be.end_time.get_time_t()));
      _ba_full_event_insert.bind_value(":status", be.status);
      _ba_full_event_insert.bind_value(":in_downtime", be.in_downtime);
      try { _ba_full_event_insert.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not insert event of BA "
               << be.ba_id << " starting at " << be.start_time
               << ": " << e.what());
      }
    }

    // Compute the associated event durations.
    _compute_event_durations(e.staticCast<bam::ba_event>(), this);
  }
  // Start of event.
  else {
    std::map<unsigned int, std::list<ba_event> >::iterator
      it(_ba_event_cache.find(be.ba_id));
    // Reopen event.
    if ((it != _ba_event_cache.end())
        && !it->second.empty()
        && ((it->second.front().end_time == be.start_time)
             || (be.end_time == (time_t)(-1)
                 && be.start_time == it->second.front().start_time))
        && (it->second.front().in_downtime == be.in_downtime)
        && (it->second.front().status == be.status)) {
      _ba_event_update.bind_value(":ba_id", be.ba_id);
      _ba_event_update.bind_value(
        ":start_time",
        static_cast<qlonglong>(it->second.front().start_time.get_time_t()));
      _ba_event_update.bind_value(
        ":end_time",
        QVariant(QVariant::LongLong));
      try { _ba_event_update.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not reopen event of BA "
               << be.ba_id << " starting at " << be.start_time << ": "
               << e.what());
      }
      it->second.front().end_time = (time_t)-1;
    }
    // Open new event.
    else {
      _ba_event_insert.bind_value(":ba_id", be.ba_id);
      _ba_event_insert.bind_value(":first_level", be.first_level);
      _ba_event_insert.bind_value(
        ":start_time",
        static_cast<qlonglong>(be.start_time.get_time_t()));
      _ba_event_insert.bind_value(":status", be.status);
      _ba_event_insert.bind_value(":in_downtime", be.in_downtime);
      try { _ba_event_insert.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not insert event of BA "
               << be.ba_id << " starting at " << be.start_time
               << ": " << e.what());
      }
      std::list<ba_event>& event_list(_ba_event_cache[be.ba_id]);
      event_list.push_front(be);
      if (event_list.size() > 2)
        event_list.pop_back();
    }
  }
  return ;
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
  _ba_duration_event_insert.bind_value(":ba_id", bde.ba_id);
  _ba_duration_event_insert.bind_value(
    ":real_start_time",
    static_cast<qlonglong>(bde.real_start_time.get_time_t()));
  _ba_duration_event_insert.bind_value(
    ":end_time",
    static_cast<qlonglong>(bde.end_time.get_time_t()));
  _ba_duration_event_insert.bind_value(
    ":start_time",
    static_cast<qlonglong>(bde.start_time.get_time_t()));
  _ba_duration_event_insert.bind_value(":duration", bde.duration);
  _ba_duration_event_insert.bind_value(
    ":sla_duration",
    bde.sla_duration);
  _ba_duration_event_insert.bind_value(
    ":timeperiod_id",
    bde.timeperiod_id);
  _ba_duration_event_insert.bind_value(
    ":timeperiod_is_default",
    bde.timeperiod_is_default);
  try { _ba_duration_event_insert.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: could not insert duration event of BA "
           << bde.ba_id << " starting at " << bde.start_time << ": "
           << e.what());
  }
  return ;
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
  // Ephemeral event.
  if (ke.start_time == ke.end_time) {
    _kpi_event_delete.bind_value(":kpi_id", ke.kpi_id);
    _kpi_event_delete.bind_value(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    try { _kpi_event_delete.run_statement(); }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM-BI: could not delete ephemeral event of KPI "
             << ke.kpi_id << " at second " << ke.start_time
             << ": " << e.what());
    }
    std::map<unsigned int, std::list<kpi_event> >::iterator
      it(_kpi_event_cache.find(ke.kpi_id));
    if ((it != _kpi_event_cache.end())
        && !it->second.empty()
        && (ke.start_time == it->second.front().start_time)) {
      it->second.pop_front();
      if (it->second.empty())
        _kpi_event_cache.erase(it);
    }
  }
  // End of event.
  else if ((ke.end_time != 0) && (ke.end_time != (time_t)-1)) {
    std::map<unsigned int, std::list<kpi_event> >::iterator
      it(_kpi_event_cache.find(ke.kpi_id));
    if (it == _kpi_event_cache.end())
      _kpi_event_cache[ke.kpi_id].push_front(ke);
    else if (it->second.empty())
      it->second.push_front(ke);
    else
      it->second.front().end_time = ke.end_time;
    _kpi_event_update.bind_value(":kpi_id", ke.kpi_id);
    _kpi_event_update.bind_value(
      ":start_time",
      static_cast<qlonglong>(ke.start_time.get_time_t()));
    _kpi_event_update.bind_value(
      ":end_time",
      static_cast<qlonglong>(ke.end_time.get_time_t()));
    try { _kpi_event_update.run_statement(); }
    catch (std::exception const& e) {
      throw (exceptions::msg() << "BAM-BI: could not close event of KPI "
             << ke.kpi_id << " starting at " << ke.start_time
             << " and ending at " << ke.end_time << ": "
             << e.what());
    }

    // If nothing was updated (can happen when there is a gap between
    // the events for some reasons, then insert a new event in the database.
    if (_kpi_event_update.num_rows_affected() == 0) {
      logging::error(logging::medium)
        << "BAM-BI: could not update the event of the KPI " << ke.kpi_id
        << " starting at " << ke.start_time.get_time_t()
        << " and ending at " << ke.end_time.get_time_t()
        << ": inserting a new event instead";
      _kpi_full_event_insert.bind_value(":kpi_id", ke.kpi_id);
      _kpi_full_event_insert.bind_value(
        ":start_time",
        static_cast<qlonglong>(ke.start_time.get_time_t()));
      _kpi_full_event_insert.bind_value(
        ":end_time",
        static_cast<qlonglong>(ke.end_time.get_time_t()));
      _kpi_full_event_insert.bind_value(":status", ke.status);
      _kpi_full_event_insert.bind_value(":in_downtime", ke.in_downtime);
      _kpi_full_event_insert.bind_value(":impact_level", ke.impact_level);
      _kpi_full_event_insert.bind_value(":output", ke.output);
      _kpi_full_event_insert.bind_value(":perfdata", ke.perfdata);
      try { _kpi_full_event_insert.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not insert event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time << ": "
               << e.what());
      }

      _kpi_event_link.bind_value(
        ":start_time",
        static_cast<qlonglong>(ke.start_time.get_time_t()));
      _kpi_event_link.bind_value(":kpi_id", ke.kpi_id);
      try { _kpi_event_link.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not create link from event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time
               << " to its associated BA event: " << e.what());
      }
    }
  }
  // Start of event.
  else {
    std::map<unsigned int, std::list<kpi_event> >::iterator
      it(_kpi_event_cache.find(ke.kpi_id));
    // Reopen event.
    if ((it != _kpi_event_cache.end())
        && !it->second.empty()
        && ((it->second.front().end_time == ke.start_time)
             || (it->second.front().end_time == (time_t)(-1)
                 && ke.start_time == it->second.front().start_time))
        && (it->second.front().in_downtime == ke.in_downtime)
        && (it->second.front().status == ke.status)
        && (it->second.front().impact_level == ke.impact_level)) {
      _kpi_event_update.bind_value(":kpi_id", ke.kpi_id);
      _kpi_event_update.bind_value(
        ":start_time",
        static_cast<qlonglong>(it->second.front().start_time.get_time_t()));
      _kpi_event_update.bind_value(
        ":end_time",
        QVariant(QVariant::LongLong));
      try { _kpi_event_update.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not reopen event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time << ": "
               << e.what());
      }
      it->second.front().end_time = (time_t)-1;
    }
    // Open new event.
    else {
      _kpi_event_insert.bind_value(":kpi_id", ke.kpi_id);
      _kpi_event_insert.bind_value(
        ":start_time",
        static_cast<qlonglong>(ke.start_time.get_time_t()));
      _kpi_event_insert.bind_value(":status", ke.status);
      _kpi_event_insert.bind_value(":in_downtime", ke.in_downtime);
      _kpi_event_insert.bind_value(":impact_level", ke.impact_level);
      _kpi_event_insert.bind_value(":output", ke.output);
      _kpi_event_insert.bind_value(":perfdata", ke.perfdata);
      try { _kpi_event_insert.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not insert event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time << ": "
               << e.what());
      }

      _kpi_event_link.bind_value(
        ":start_time",
        static_cast<qlonglong>(ke.start_time.get_time_t()));
      _kpi_event_link.bind_value(":kpi_id", ke.kpi_id);
      try { _kpi_event_link.run_statement(); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not create link from event of KPI "
               << ke.kpi_id << " starting at " << ke.start_time
               << " to its associated BA event: " << e.what());
      }

      std::list<kpi_event>& event_list(_kpi_event_cache[ke.kpi_id]);
      event_list.push_front(ke);
      if (event_list.size() > 2)
        event_list.pop_back();
    }
  }
  return ;
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
  _dimension_ba_insert.bind_value(":ba_id", dba.ba_id);
  _dimension_ba_insert.bind_value(":ba_name", dba.ba_name);
  _dimension_ba_insert.bind_value(
                         ":ba_description",
                         dba.ba_description);
  _dimension_ba_insert.bind_value(
                         ":sla_month_percent_crit",
                         dba.sla_month_percent_crit);
  _dimension_ba_insert.bind_value(
                         ":sla_month_percent_warn",
                         dba.sla_month_percent_warn);
  _dimension_ba_insert.bind_value(
                         ":sla_month_duration_crit",
                         dba.sla_duration_crit);
  _dimension_ba_insert.bind_value(
                         ":sla_month_duration_warn"
                         , dba.sla_duration_warn);
  try { _dimension_ba_insert.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg() << "BAM-BI: could not insert BA "
           << dba.ba_id << ": " << e.what());
  }
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
  _dimension_bv_insert.bind_value(":bv_id", dbv.bv_id);
  _dimension_bv_insert.bind_value(":bv_name", dbv.bv_name);
  _dimension_bv_insert.bind_value(
                         ":bv_description",
                         dbv.bv_description);
  try { _dimension_bv_insert.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg() << "BAM-BI: could not insert BV "
           << dbv.bv_id << ": " << e.what());
  }
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
  _dimension_ba_bv_relation_insert.bind_value(":ba_id", dbabv.ba_id);
  _dimension_ba_bv_relation_insert.bind_value(":bv_id", dbabv.bv_id);
  try { _dimension_ba_bv_relation_insert.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: could not insert dimension of BA-BV relation "
           << dbabv.ba_id << "-"<< dbabv.bv_id << ": " << e.what());
  }
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
      std::auto_ptr<QMutexLocker> lock(_availabilities->lock());

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
      _db.commit();
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

    for (std::vector<misc::shared_ptr<database_query> >::iterator
           it(_dimension_truncate_tables.begin()),
           end(_dimension_truncate_tables.end());
         it != end;
         ++it)
      (*it)->run_statement(
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
  _dimension_kpi_insert.bind_value(":kpi_id", dk.kpi_id);
  _dimension_kpi_insert.bind_value(":kpi_name", kpi_name);
  _dimension_kpi_insert.bind_value(":ba_id", dk.ba_id);
  _dimension_kpi_insert.bind_value(":ba_name", dk.ba_name);
  _dimension_kpi_insert.bind_value(":host_id", dk.host_id);
  _dimension_kpi_insert.bind_value(":host_name", dk.host_name);
  _dimension_kpi_insert.bind_value(":service_id", dk.service_id);
  _dimension_kpi_insert.bind_value(
                          ":service_description",
                          dk.service_description);
  _dimension_kpi_insert.bind_value(
                          ":kpi_ba_id",
                          (dk.kpi_ba_id != 0
                           ? dk.kpi_ba_id
                           : QVariant(QVariant::UInt)));
  _dimension_kpi_insert.bind_value(":kpi_ba_name", dk.kpi_ba_name);
  _dimension_kpi_insert.bind_value(
                          ":meta_service_id",
                          dk.meta_service_id);
  _dimension_kpi_insert.bind_value(
                          ":meta_service_name",
                          dk.meta_service_name);
  _dimension_kpi_insert.bind_value(
                          ":impact_warning",
                          dk.impact_warning);
  _dimension_kpi_insert.bind_value(
                          ":impact_critical",
                          dk.impact_critical);
  _dimension_kpi_insert.bind_value(
                          ":impact_unknown",
                          dk.impact_unknown);
  _dimension_kpi_insert.bind_value(":boolean_id", dk.boolean_id);
  _dimension_kpi_insert.bind_value(":boolean_name", dk.boolean_name);
  try { _dimension_kpi_insert.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg() << "BAM-BI: could not insert dimension of KPI "
           << dk.kpi_id << ": " << e.what());
  }
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
  database_query& q(_dimension_timeperiod_insert);
  q.bind_value(":timeperiod_id", tp.id);
  q.bind_value(":name", tp.name);
  q.bind_value(":sunday", tp.sunday);
  q.bind_value(":monday", tp.monday);
  q.bind_value(":tuesday", tp.tuesday);
  q.bind_value(":wednesday", tp.wednesday);
  q.bind_value(":thursday", tp.thursday);
  q.bind_value(":friday", tp.friday);
  q.bind_value(":saturday", tp.saturday);
  try { q.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg() << "BAM-BI: could not insert timeperiod "
           << tp.id << " ('" << tp.name << "'): " << e.what());
  }
  _apply(tp);
  return ;
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
  database_query& q(_dimension_timeperiod_exception_insert);
  q.bind_value(":timeperiod_id", tpe.timeperiod_id);
  q.bind_value(":daterange", tpe.daterange);
  q.bind_value(":timerange", tpe.timerange);
  try { q.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: could not insert exception of timeperiod "
           << tpe.timeperiod_id << ": " << e.what());
  }
  _apply(tpe);
  return ;
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
  database_query& q(_dimension_timeperiod_exclusion_insert);
  q.bind_value(":timeperiod_id", tpe.timeperiod_id);
  q.bind_value(":excluded_timeperiod_id", tpe.excluded_timeperiod_id);
  try { q.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: could not insert exclusion of timeperiod "
           << tpe.excluded_timeperiod_id << " by timeperiod "
           << tpe.timeperiod_id << ": " << e.what());
  }
  _apply(tpe);
  return ;
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
  database_query& q(_dimension_ba_timeperiod_insert);
  q.bind_value(":ba_id", r.ba_id);
  q.bind_value(":timeperiod_id", r.timeperiod_id);
  q.bind_value(":is_default", r.is_default);
  try { q.run_statement(); }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: could not insert relation of BA "
           << r.ba_id << " to timeperiod " << r.timeperiod_id << ": "
           << e.what());
  }
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

  if (timeperiods.empty())
    return ;

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
    if (dur_ev->start_time < dur_ev->end_time) {
      dur_ev->duration = dur_ev->end_time - dur_ev->start_time;
      dur_ev->sla_duration = tp->duration_intersect(
                                   dur_ev->start_time,
                                   dur_ev->end_time);
      dur_ev->timeperiod_id = tp->get_id();
      dur_ev->timeperiod_is_default = is_default;
      visitor->write(dur_ev.staticCast<io::data>());
    }
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
    std::auto_ptr<QMutexLocker> lock(_availabilities->lock());

    // Delete obsolete ba events durations.
    {
      std::string query;
      query.append(
        "DELETE a"
        "  FROM mod_bam_reporting_ba_events_durations as a"
        "    INNER JOIN mod_bam_reporting_ba_events as b"
        "      ON a.ba_event_id = b.ba_event_id"
        "  WHERE b.ba_id IN (");
      query.append(r.bas_to_rebuild.toStdString());
      query.append(")");
      database_query q(_db);
      try { q.run_query(query); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not delete BA durations "
               << r.bas_to_rebuild << ": " << e.what());
      }
    }

    // Get the ba events.
    std::vector<misc::shared_ptr<ba_event> > ba_events;
    {
      std::string query;
      query.append(
              "SELECT ba_id, start_time, end_time, "
              "       status, in_downtime boolean"
              "  FROM mod_bam_reporting_ba_events"
              "  WHERE end_time IS NOT NULL"
              "    AND ba_id IN (");
      query.append(r.bas_to_rebuild.toStdString());
      query.append(")");
      database_query q(_db);
      try { q.run_query(query); }
      catch (std::exception const& e) {
        throw (exceptions::msg()
               << "BAM-BI: could not get BA events of "
               << r.bas_to_rebuild << " :" << e.what());
      }
      while (q.next()) {
        misc::shared_ptr<ba_event> baev(new ba_event);
        baev->ba_id = q.value(0).toInt();
        baev->start_time = q.value(1).toInt();
        baev->end_time = q.value(2).toInt();
        baev->status = q.value(3).toInt();
        baev->in_downtime = q.value(4).toBool();
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
