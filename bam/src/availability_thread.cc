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
#include <QMutexLocker>
//#include <QVariant>
#include <sstream>
#include "com/centreon/broker/bam/availability_thread.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg       Database configuration.
 *  @param[in] shared_map   A timeperiod map shared with the reporting.
 */
availability_thread::availability_thread(
                       database_config const& db_cfg,
                       timeperiod_map& shared_map)
  : _db_cfg(db_cfg),
    _shared_tps(shared_map),
    _mutex(QMutex::NonRecursive),
    _should_exit(false),
    _should_rebuild_all(false) {}

/**
 *  Destructor.
 */
availability_thread::~availability_thread() {
  _close_database();
}

/**
 *  The main loop of thread.
 */
void availability_thread::run() {
  // Lock the mutex.
  QMutexLocker lock(&_mutex);
  // Release the thread that is waiting on our start.
  _started.release();

  // Check for termination asked.
  if (_should_exit)
    return ;

  while (true) {
    try {
      // Calculate the duration until next midnight.
      time_t midnight = _compute_next_midnight();
      unsigned long wait_for = std::difftime(midnight, ::time(NULL));
      logging::debug(logging::medium)
          << "BAM-BI: availability thread sleeping for "
          << wait_for << " seconds.";
      _wait.wait(lock.mutex(), wait_for * 1000);
      logging::debug(logging::medium)
          << "BAM-BI: availability thread waking up ";

      // Termination asked.
      if (_should_exit)
        break ;

      // Open the database.
      _open_database();

      _build_availabilities(_compute_start_of_day(::time(NULL)));
      _should_rebuild_all = false;
      _bas_to_rebuild.clear();

      // Close the database.
      _close_database();
    }
    catch (std::exception const& e) {
      // Something bad happened. Wait for the next loop.
      logging::error(logging::medium) << e.what();
      _close_database();
    }
  }
}

/**
 *  Ask for the thread termination.
 */
void availability_thread::terminate() {
  QMutexLocker lock(&_mutex);
  _should_exit = true;
  _wait.wakeOne();
}

/**
 *  Start a thread, and wait for its initialization.
 */
void availability_thread::start_and_wait() {
  if (!isRunning()) {
    start();
    _started.acquire();
  }
}


/**
 *  Lock the main mutex of the availability thread.
 *
 *  @return  A QMutexLocker locking the main mutex.
 */
std::unique_ptr<QMutexLocker> availability_thread::lock() {
  return (std::unique_ptr<QMutexLocker>(new QMutexLocker(&_mutex)));
}

/**
 *  Ask the thread to rebuild the availabilities.
 *
 *  @param[in] bas_to_rebuild  A string containing the bas to rebuild.
 */
void availability_thread::rebuild_availabilities(
                            QString const& bas_to_rebuild) {
  QMutexLocker lock(&_mutex);
  if (bas_to_rebuild.isEmpty())
    return ;
  _should_rebuild_all = true;
  _bas_to_rebuild = bas_to_rebuild;
  _wait.wakeOne();
}

/**
 *  Delete all the availabilities.
 */
void availability_thread::_delete_all_availabilities() {
  logging::debug(logging::low)
    << "BAM-BI: availability thread deleting availabilities";

  // Prepare the query.
  std::stringstream query;
  query << "DELETE FROM mod_bam_reporting_ba_availabilities WHERE ba_id IN ("
        << _bas_to_rebuild.toStdString() << ")";

  _mysql->run_query(
      query.str(),
      "BAM-BI: availability thread could not delete the "
      "BA availabilities from the reporting database");
}

/**
 *  @brief  Build all the availabilities.
 *
 *  This is called from the context of the availability thread.
 *
 *  @param[in] mignight   Midnight of today.
 */
void availability_thread::_build_availabilities(time_t midnight) {
  time_t first_day = 0;
  time_t last_day = midnight;
  std::stringstream query;
  int thread_id;

  // Get the first day of rebuilding. If a complete rebuilding was asked,
  // it's the day of the chronogically first event to rebuild.
  // If not, it's the day following the chronogically last availability.
  if (_should_rebuild_all) {
    query << "SELECT MIN(start_time), MAX(end_time), MIN(IFNULL(end_time, '0'))"
             "  FROM mod_bam_reporting_ba_events"
             "  WHERE ba_id IN (" << _bas_to_rebuild.toStdString() << ")";
    try {
      std::promise<mysql_result> promise;
      thread_id = _mysql->run_query_and_get_result(query.str(), &promise);
      mysql_result res(promise.get_future().get());
      if (!_mysql->fetch_row(res))
        throw (exceptions::msg() << "no events matching BAs to rebuild");
      first_day = res.value_as_i32(0);
      first_day = _compute_start_of_day(first_day);
      // If there is opened events, rebuild until midnight of this day.
      // If not, rebuild until the last closed events.
      if (res.value_as_i32(2) != 0)
        last_day = _compute_start_of_day(res.value_as_f64(1));

      //FIXME DBR: why this line?
      _mysql->fetch_row(res);
      _delete_all_availabilities();
    }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM-BI: availability thread could not select the BA durations "
                "from the reporting database: " << e.what());
    }

  }
  else {
    query << "SELECT MAX(time_id)"
             "  FROM mod_bam_reporting_ba_availabilities";
    try {
      std::promise<mysql_result> promise;
      thread_id = _mysql->run_query_and_get_result(query.str(), &promise);
      //FIXME DBR: to finish...
      mysql_result res(promise.get_future().get());
      if (!_mysql->fetch_row(res))
        throw (exceptions::msg() << "no availability in table");
      first_day = res.value_as_i32(0);
      first_day = time::timeperiod::add_round_days_to_midnight(
                                      first_day, 3600 * 24);
      _mysql->fetch_row(res);
    }
    catch (std::exception const& e) {
      throw (exceptions::msg()
             << "BAM-BI: availability thread "
                "could not select the BA availabilities "
                "from the reporting database: " << e.what());
    }
  }

  logging::debug(logging::medium)
    << "BAM-BI: availability thread writing availabilities from: "
    << first_day << " to " << last_day;

  // Write the availabilities day after day.
  while (first_day < last_day) {
    time_t next_day =
      time::timeperiod::add_round_days_to_midnight(first_day, 3600 * 24);
    _build_daily_availabilities(thread_id, first_day, next_day);
    first_day = next_day;
  }
}

/**
 *  @brief  Build all the availabilities of a day.
 *
 *  This is called from the context of the availability thread.
 *
 *  @param[in] q         A SQL query object.
 *  @param[in] day_start The start of the day.
 *  @param[in] day_end   The first second of the next day.
 */
void availability_thread::_build_daily_availabilities(
                            int thread_id,
                            time_t day_start,
                            time_t day_end) {
  logging::info(logging::medium)
      << "BAM-BI: availability thread writing daily availability for "
         "day : " << day_start << "-" << day_end;

  // Build the availabilities tied to event durations (event finished)
  std::stringstream query;
  query << "SELECT a.ba_event_id, b.ba_id, a.start_time, a.end_time,"
           "       a.duration, a.sla_duration, a.timeperiod_id,"
           "       a.timeperiod_is_default, b.status, b.in_downtime"
           "  FROM mod_bam_reporting_ba_events_durations AS a"
           "    INNER JOIN mod_bam_reporting_ba_events AS b"
           "  ON a.ba_event_id = b.ba_event_id"
           "  WHERE ";
  if (_should_rebuild_all)
    query << "(b.ba_id IN (" << _bas_to_rebuild.toStdString() << ")) AND ";
  query << "((a.start_time BETWEEN " << day_start << " AND " << day_end - 1
        << ") OR (a.end_time BETWEEN " << day_start << " AND " << day_end - 1
        << ") OR (" << day_start << " BETWEEN a.start_time AND a.end_time))";

  std::promise<mysql_result> promise;
  _mysql->run_query_and_get_result(
      query.str(),
      &promise,
      "BAM-BI: availability thread could not build the data", thread_id);

  // Create a builder for each ba_id and associated timeperiod_id.
  std::map<std::pair<unsigned int, unsigned int>,
            availability_builder> builders;
  mysql_result res(promise.get_future().get());
  while (_mysql->fetch_row(res)) {
    unsigned int ba_id = res.value_as_i32(1);
    unsigned int timeperiod_id = res.value_as_i32(6);
    // Find the timeperiod.
    time::timeperiod::ptr tp = _shared_tps.get_timeperiod(timeperiod_id);
    // No timeperiod found, skip.
    if (!tp)
      continue ;
    // Find the builder.
    std::map<std::pair<unsigned int, unsigned int>,
              availability_builder>::iterator found
        = builders.find(std::make_pair(ba_id, timeperiod_id));
    // No builders found, create one.
    if (found == builders.end())
      found = builders.insert(
                std::make_pair(
                  std::make_pair(ba_id, timeperiod_id),
                  availability_builder(day_end, day_start))).first;
    // Add the event to the builder.
    found->second.add_event(
      res.value_as_i32(8),  // Status
      res.value_as_i32(2),  // Start time
      res.value_as_i32(3),  // End time
      res.value_as_bool(9), // Was in downtime
      tp);
    // Add the timeperiod is default flag.
    found->second.set_timeperiod_is_default(res.value_as_bool(7));
  }

  // Build the availabilities tied to event not finished.
  query.str("");
  query << "SELECT ba_event_id, ba_id, start_time, end_time,"
           "       status, in_downtime"
           "  FROM mod_bam_reporting_ba_events"
           "  WHERE ";
  if (_should_rebuild_all)
    query << "(ba_id IN (" << _bas_to_rebuild.toStdString() << ")) AND ";
  query << "(start_time < " << day_end << " AND end_time IS NULL)";

  promise = std::promise<mysql_result>();
  _mysql->run_query_and_get_result(
      query.str(),
      &promise,
      "BAM-BI: availability thread could not build the data: ",
      thread_id);

  res = promise.get_future().get();
  while (_mysql->fetch_row(res)) {
    unsigned int ba_id = res.value_as_i32(1);
    // Get all the timeperiods associated with the ba of this event.
    std::vector<std::pair<time::timeperiod::ptr, bool> >
      tps = _shared_tps.get_timeperiods_by_ba_id(ba_id);
    for (std::vector<std::pair<time::timeperiod::ptr, bool> >::const_iterator
           it(tps.begin()),
           end(tps.end());
         it != end;
         ++it) {
      unsigned int tp_id = it->first->get_id();
      // Find the builder.
      std::map<std::pair<unsigned int, unsigned int>,
                availability_builder>::iterator found
          = builders.find(std::make_pair(ba_id, tp_id));
      // No builders found, create one.
      if (found == builders.end())
        found = builders.insert(
                  std::make_pair(
                    std::make_pair(ba_id, tp_id),
                    availability_builder(day_end, day_start))).first;
      // Add the event to the builder.
      found->second.add_event(
        res.value_as_i32(4),  // Status
        res.value_as_i32(2),  // Start time
        res.value_as_i32(3),  // End time
        res.value_as_bool(5), // Was in downtime
        it->first);
      // Add the timeperiod is default flag.
      found->second.set_timeperiod_is_default(it->second);
    }
  }

  // For each builder, write the availabilities.
  for (std::map<std::pair<unsigned int, unsigned int>,
                availability_builder>::const_iterator
         it = builders.begin(),
         end = builders.end();
       it != end;
       ++it)
    _write_availability(thread_id, it->second, it->first.first, day_start, it->first.second);
}

/**
 *  Write an availability to the database.
 *
 *  @param[in] q                      A QSqlQuery connected to this database.
 *  @param[in] builder                The builder of an availability.
 *  @param[in] ba_id                  The id of the ba.
 *  @param[in] day_start              The start of the day.
 *  @param[in] timeperiod_id          The id of the timeperiod.
 */
void availability_thread::_write_availability(
                            int thread_id,
                            availability_builder const& builder,
                            unsigned int ba_id,
                            time_t day_start,
                            unsigned int timeperiod_id) {
  logging::debug(logging::low)
      << "BAM-BI: availability thread writing availability for "
         "BA " << ba_id << " at day " << day_start
      << " (timeperiod " << timeperiod_id << ")";
  std::stringstream query;
  query << "INSERT INTO mod_bam_reporting_ba_availabilities "
        << "  (ba_id, time_id, timeperiod_id, timeperiod_is_default,"
           "   available, unavailable, degraded,"
           "   unknown, downtime, alert_unavailable_opened,"
           "   alert_degraded_opened, alert_unknown_opened,"
           "   nb_downtime)"
           "  VALUES (" << ba_id << ", " << day_start << ", "
        << timeperiod_id << ", " << builder.get_timeperiod_is_default() << ", "
        << builder.get_available() << ", " << builder.get_unavailable()
        << ", " << builder.get_degraded() << ", " << builder.get_unknown()
        << ", " << builder.get_downtime() << ", "
        << builder.get_unavailable_opened() << ", "
        << builder.get_degraded_opened() << ", " << builder.get_unknown_opened()
        << ", " << builder.get_downtime_opened() << ")";
  _mysql->run_query(
      query.str(),
      "BAM-BI: availability thread could not insert an availability: ",
      true,
      thread_id);
}

/**
 *  Compute the next midnight.
 *
 *  @return  The next midnight.
 */
time_t availability_thread::_compute_next_midnight() {
  return (time::timeperiod::add_round_days_to_midnight(
                              _compute_start_of_day(::time(NULL)),
                              3600 * 24));
}

/**
 *  Get the start of the day of the timestamp when.
 *
 *  @param[in] when  The timestamp.
 *
 *  @return  The result.
 */
time_t availability_thread::_compute_start_of_day(
                            time_t when) {
  struct tm tmv;
  if (!localtime_r(&when, &tmv))
    throw (exceptions::msg()
           << "BAM-BI: availability thread could not compute start of day");
  tmv.tm_sec = tmv.tm_min = tmv.tm_hour = 0;
  return (mktime(&tmv));
}

/**
 *  Open the database.
 */
void availability_thread::_open_database() {
  // Availability thread connection ID.
  QString bam_id;
  bam_id.setNum((qulonglong)this, 16);

  // Add database connection.
  try {
    _mysql.reset(new mysql(_db_cfg));
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "BAM-BI: availability thread could not connect to "
              "reporting database '" << e.what());
  }
}

/**
 *  Close the database.
 */
void availability_thread::_close_database() {
  if (_mysql.get()) {
    _mysql.reset();
  }
}
