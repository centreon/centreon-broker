/*
** Copyright 2014, 2021 Centreon
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

#include "com/centreon/broker/bam/availability_thread.hh"

#include <ctime>

#include "com/centreon/broker/database/mysql_error.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg       Database configuration.
 *  @param[in] shared_map   A timeperiod map shared with the reporting.
 */
availability_thread::availability_thread(database_config const& db_cfg,
                                         timeperiod_map& shared_map)
    : _started_flag{false},
      _db_cfg(db_cfg),
      _shared_tps(shared_map),
      _mutex{},
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
  std::unique_lock<std::mutex> lock(_mutex);

  // Check for termination asked.
  if (_should_exit)
    return;

  for (;;) {
    try {
      // Calculate the duration until next midnight.
      time_t midnight = _compute_next_midnight();
      unsigned long wait_for = std::difftime(midnight, ::time(nullptr));
      log_v2::bam()->debug(
          "BAM-BI: availability thread sleeping for {} seconds.", wait_for);
      _wait.wait_for(lock, std::chrono::seconds(wait_for));
      log_v2::bam()->debug("BAM-BI: availability thread waking up ");

      // Termination asked.
      if (_should_exit)
        break;

      log_v2::bam()->debug("BAM-BI: opening database");
      // Open the database.
      _open_database();

      log_v2::bam()->debug("BAM-BI: build availabilities");
      _build_availabilities(_compute_start_of_day(::time(nullptr)));
      _should_rebuild_all = false;
      _bas_to_rebuild.clear();

      // Close the database.
      _close_database();
      log_v2::bam()->debug("BAM-BI: database closed");
    } catch (const std::exception& e) {
      // Something bad happened. Wait for the next loop.
      log_v2::bam()->error("BAM-BI: Something went wrong: {}", e.what());
      logging::error(logging::medium) << e.what();
      _close_database();
    }
  }
}

/**
 *  Ask for the thread termination.
 */
void availability_thread::terminate() {
  std::lock_guard<std::mutex> lock(_mutex);
  _should_exit = true;
  _wait.notify_one();
}

/**
 *  Start a thread, and wait for its initialization.
 */
void availability_thread::start_and_wait() {
  if (!_started_flag) {
    _thread = std::thread(&availability_thread::run, this);
    _started_flag = true;
  }
}

void availability_thread::wait() {
  _thread.join();
  _started_flag = false;
}

/**
 *  @brief Lock the main mutex of the availability thread.
 */
void availability_thread::lock() {
  _mutex.lock();
}

/**
 * @brief Unlock the main mutex of the availability thread.
 */
void availability_thread::unlock() {
  _mutex.unlock();
}

/**
 *  Ask the thread to rebuild the availabilities.
 *
 *  @param[in] bas_to_rebuild  A string containing the bas to rebuild.
 */
void availability_thread::rebuild_availabilities(
    std::string const& bas_to_rebuild) {
  std::lock_guard<std::mutex> lock(_mutex);
  if (bas_to_rebuild.empty())
    return;
  _should_rebuild_all = true;
  _bas_to_rebuild = bas_to_rebuild;
  _wait.notify_one();
}

/**
 *  Delete all the availabilities.
 */
void availability_thread::_delete_all_availabilities() {
  log_v2::bam()->debug("BAM-BI: availability thread deleting availabilities");

  // Prepare the query.
  std::string query_str(fmt::format(
      "DELETE FROM mod_bam_reporting_ba_availabilities WHERE ba_id IN ({})",
      _bas_to_rebuild));

  _mysql->run_query(query_str, database::mysql_error::delete_availabilities);
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
  std::string query_str;
  int thread_id;

  // Get the first day of rebuilding. If a complete rebuilding was asked,
  // it's the day of the chronogically first event to rebuild.
  // If not, it's the day following the chronogically last availability.
  if (_should_rebuild_all) {
    query_str = fmt::format(
        "SELECT MIN(start_time), MAX(end_time), MIN(IFNULL(end_time, '0'))"
        "  FROM mod_bam_reporting_ba_events"
        "  WHERE ba_id IN ({})",
        _bas_to_rebuild);
    try {
      std::promise<database::mysql_result> promise;
      thread_id = _mysql->run_query_and_get_result(query_str, &promise);
      database::mysql_result res(promise.get_future().get());
      if (!_mysql->fetch_row(res))
        throw msg_fmt("no events matching BAs to rebuild");
      first_day = res.value_as_i32(0);
      first_day = _compute_start_of_day(first_day);
      // If there is opened events, rebuild until midnight of this day.
      // If not, rebuild until the last closed events.
      if (res.value_as_i32(2) != 0)
        last_day = _compute_start_of_day(res.value_as_f64(1));

      _delete_all_availabilities();
    } catch (const std::exception& e) {
      log_v2::bam()->error(
          "BAM-BI: availability thread could not select the BA durations from "
          "the reporting database: {}",
          e.what());
      throw msg_fmt(
          "BAM-BI: availability thread could not select the BA durations "
          "from the reporting database: {}",
          e.what());
    }

  } else {
    query_str = "SELECT MAX(time_id) FROM mod_bam_reporting_ba_availabilities";
    try {
      std::promise<database::mysql_result> promise;
      thread_id = _mysql->run_query_and_get_result(query_str, &promise);
      database::mysql_result res(promise.get_future().get());
      if (!_mysql->fetch_row(res)) {
        log_v2::bam()->error("no availability in table");
        throw msg_fmt("no availability in table");
      }
      first_day = res.value_as_i32(0);
      first_day =
          time::timeperiod::add_round_days_to_midnight(first_day, 3600 * 24);
    } catch (const std::exception& e) {
      std::string msg(fmt::format(
          "BAM-BI: availability thread could not select the BA availabilities "
          "from the reporting database: {}",
          e.what()));
      log_v2::bam()->error(msg);
      throw msg_fmt(msg);
    }
  }

  log_v2::bam()->debug(
      "BAM-BI: availability thread writing availabilities from: {} to {}",
      first_day, last_day);

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
void availability_thread::_build_daily_availabilities(int thread_id,
                                                      time_t day_start,
                                                      time_t day_end) {
  log_v2::bam()->info(
      "BAM-BI: availability thread writing daily availability for day : {}-{}",
      day_start, day_end);

  // Build the availabilities tied to event durations (event finished)
  std::string query(fmt::format(
      "SELECT a.ba_event_id, b.ba_id, a.start_time, a.end_time, a.duration, "
      "a.sla_duration, a.timeperiod_id, a.timeperiod_is_default, b.status, "
      "b.in_downtime FROM mod_bam_reporting_ba_events_durations AS a INNER "
      "JOIN mod_bam_reporting_ba_events AS b ON a.ba_event_id=b.ba_event_id "
      "AND b.end_time IS NOT NULL WHERE a.start_time<{} AND a.end_time>={} "
      "{}",
      day_end, day_start,
      _should_rebuild_all ? fmt::format("AND b.ba_id IN({})", _bas_to_rebuild)
                          : ""));
  log_v2::bam()->debug("Query: {}", query);

  log_v2::bam()->debug("Query: {}", query);
  std::promise<database::mysql_result> promise;
  _mysql->run_query_and_get_result(query, &promise, thread_id);

  // Create a builder for each ba_id and associated timeperiod_id.
  std::map<std::pair<uint32_t, uint32_t>, availability_builder> builders;
  try {
    database::mysql_result res(promise.get_future().get());
    while (_mysql->fetch_row(res)) {
      uint32_t ba_id = res.value_as_i32(1);
      uint32_t timeperiod_id = res.value_as_i32(6);
      // Find the timeperiod.
      time::timeperiod::ptr tp = _shared_tps.get_timeperiod(timeperiod_id);
      // No timeperiod found, skip.
      if (!tp) {
        log_v2::bam()->debug("no timeperiod found with id {}", timeperiod_id);
        continue;
      }
      // Find the builder.
      std::map<std::pair<uint32_t, uint32_t>, availability_builder>::iterator
          found = builders.find(std::make_pair(ba_id, timeperiod_id));
      // No builders found, create one.
      if (found == builders.end()) {
        log_v2::bam()->debug(
            "no builder found for ba id {} and timeperiod id {}: Adding it",
            ba_id, timeperiod_id);
        found = builders
                    .insert(std::make_pair(
                        std::make_pair(ba_id, timeperiod_id),
                        availability_builder(day_end, day_start)))
                    .first;
      }
      // Add the event to the builder.
      found->second.add_event(res.value_as_i32(8),   // Status
                              res.value_as_i32(2),   // Start time
                              res.value_as_i32(3),   // End time
                              res.value_as_bool(9),  // Was in downtime
                              tp);
      // Add the timeperiod is default flag.
      found->second.set_timeperiod_is_default(res.value_as_bool(7));
    }
  } catch (const std::exception& e) {
    throw msg_fmt("BAM-BI: availability thread could not build the data {}",
                  e.what());
  }

  log_v2::bam()->debug("{} builders of availabilities created",
                       builders.size());

  // Build the availabilities tied to event not finished.
  query = fmt::format(
      "SELECT ba_event_id,ba_id,start_time,end_time,status,"
      "in_downtime FROM mod_bam_reporting_ba_events WHERE start_time<{} AND "
      "end_time IS NULL {}",
      day_end,
      _should_rebuild_all ? fmt::format("AND ba_id IN ({})", _bas_to_rebuild)
                          : "");
  log_v2::bam()->debug("Query: {}", query);

  promise = std::promise<database::mysql_result>();
  _mysql->run_query_and_get_result(query, &promise, thread_id);

  try {
    database::mysql_result res(promise.get_future().get());
    while (_mysql->fetch_row(res)) {
      uint32_t ba_id = res.value_as_i32(1);
      // Get all the timeperiods associated with the ba of this event.
      std::vector<std::pair<time::timeperiod::ptr, bool>> tps =
          _shared_tps.get_timeperiods_by_ba_id(ba_id);
      int count = 0;
      for (auto it = tps.begin(), end = tps.end(); it != end; ++it) {
        uint32_t tp_id = it->first->get_id();
        // Find the builder.
        auto found = builders.find(std::make_pair(ba_id, tp_id));
        // No builders found, create one.
        if (found == builders.end()) {
          found = builders
                      .insert(std::make_pair(
                          std::make_pair(ba_id, tp_id),
                          availability_builder(day_end, day_start)))
                      .first;
          count++;
        }
        // Add the event to the builder.
        found->second.add_event(res.value_as_i32(4),   // Status
                                res.value_as_i32(2),   // Start time
                                res.value_as_i32(3),   // End time
                                res.value_as_bool(5),  // Was in downtime
                                it->first);
        // Add the timeperiod is default flag.
        found->second.set_timeperiod_is_default(it->second);
      }
      log_v2::bam()->debug("{} builder(s) were missing for ba {}", count,
                           ba_id);
    }
  } catch (const std::exception& e) {
    throw msg_fmt("BAM-BI: availability thread could not build the data: {}",
                  e.what());
  }

  log_v2::bam()->debug("{} builder(s) to write availabilities",
                       builders.size());
  // For each builder, write the availabilities.
  for (auto it = builders.begin(), end = builders.end(); it != end; ++it)
    _write_availability(thread_id, it->second, it->first.first, day_start,
                        it->first.second);
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
    uint32_t ba_id,
    time_t day_start,
    uint32_t timeperiod_id) {
  log_v2::bam()->debug(
      "BAM-BI: availability thread writing availability for BA {} at day {} "
      "(timeperiod {})",
      ba_id, day_start, timeperiod_id);

  std::string query_str(fmt::format(
      "INSERT INTO mod_bam_reporting_ba_availabilities "
      "(ba_id, time_id, timeperiod_id, timeperiod_is_default,"
      " available, unavailable, degraded,"
      " unknown, downtime, alert_unavailable_opened,"
      " alert_degraded_opened, alert_unknown_opened,"
      " nb_downtime)"
      " VALUES ({},{},{},{},{},{},{},{},{},{},{},{},{})",
      ba_id, day_start, timeperiod_id, builder.get_timeperiod_is_default(),
      builder.get_available(), builder.get_unavailable(),
      builder.get_degraded(), builder.get_unknown(), builder.get_downtime(),
      builder.get_unavailable_opened(), builder.get_degraded_opened(),
      builder.get_unknown_opened(), builder.get_downtime_opened()));

  log_v2::bam()->debug("Query: {}", query_str);
  _mysql->run_query(query_str, database::mysql_error::insert_availability, true,
                    thread_id);
}

/**
 *  Compute the next midnight.
 *
 *  @return  The next midnight.
 */
time_t availability_thread::_compute_next_midnight() {
  return time::timeperiod::add_round_days_to_midnight(
      _compute_start_of_day(::time(nullptr)), 3600 * 24);
}

/**
 *  Get the start of the day of the timestamp when.
 *
 *  @param[in] when  The timestamp.
 *
 *  @return  The result.
 */
time_t availability_thread::_compute_start_of_day(time_t when) {
  struct tm tmv;
  if (!localtime_r(&when, &tmv))
    throw msg_fmt("BAM-BI: availability thread could not compute start of day");
  tmv.tm_sec = tmv.tm_min = tmv.tm_hour = 0;
  return mktime(&tmv);
}

/**
 *  Open the database.
 */
void availability_thread::_open_database() {
  // Add database connection.
  try {
    _mysql.reset(new mysql(_db_cfg));
  } catch (const std::exception& e) {
    throw msg_fmt(
        "BAM-BI: availability thread could not connect to "
        "reporting database '{}'",
        e.what());
  }
}

/**
 *  Close the database.
 */
void availability_thread::_close_database() {
  if (_mysql) {
    _mysql.reset();
  }
}
