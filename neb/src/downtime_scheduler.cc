/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/neb/downtime_scheduler.hh"
#include <ctime>
#include <limits>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
downtime_scheduler::downtime_scheduler()
    : _should_exit(false), _started_flag{false} {}

/**
 *  Called by the downtime manager thread when it starts.
 */
void downtime_scheduler::run() {
  std::lock_guard<std::mutex> lock(_general_mutex);

  while (1) {
    // Wait until the first downtime in the queue - or forever until awakened
    // if none.
    time_t first_time =
        std::min(_get_first_timestamp(_downtime_starts),
                 _get_first_timestamp(_downtime_ends), timestamp::less);
    time_t now = ::time(nullptr);
    unsigned long wait_for = first_time == time_t(-1)
                                 ? std::numeric_limits<unsigned long>::max()
                             : (first_time >= now) ? (first_time - now) * 1000
                                                   : 0;

    logging::debug(logging::medium)
        << "node events: downtime scheduler sleeping for " << wait_for / 1000.0
        << " seconds";

    std::unique_lock<std::mutex> lock(_general_mutex);
    _general_condition.wait_for(lock, std::chrono::milliseconds(wait_for));

    logging::debug(logging::medium)
        << "node events: downtime scheduler waking up";

    // The should exit flag was set - exit.
    if (_should_exit)
      break;

    // Process the downtimes and release the mutex.
    _process_downtimes();
  }
}

/**
 *  Start the downtime scheduler and wait until it has started.
 */
void downtime_scheduler::start_and_wait() {
  _thread = std::thread(&downtime_scheduler::run, this);
  _started_flag = true;
}

/**
 *  Ask gracefully for the downtime scheduling thread to exit.
 */
void downtime_scheduler::quit() throw() {
  // Set the should exit flag.
  {
    std::lock_guard<std::mutex> lock(_general_mutex);
    _should_exit = true;
    // Wake the notification scheduling thread.
    _general_condition.notify_all();
  }
}

/**
 *  Add a downtime to the list of managed downtimes.
 *
 *  @param[in] start_time  The start of the scheduling.
 *  @param[in] end_time    The end of the scheduling.
 *  @param[in] dwn         The downtime.
 */
void downtime_scheduler::add_downtime(timestamp start_time,
                                      timestamp end_time,
                                      downtime const& dwn) {
  if (dwn.start_time >= dwn.end_time) {
    logging::debug(logging::medium)
        << "node events: attempt to schedule a downtime when start time "
           "is superior or equal to its end time";
    return;
  }

  // Lock the mutex.
  std::lock_guard<std::mutex> lock(_general_mutex);

  timestamp first_starting_timestamp = _get_first_timestamp(_downtime_starts);
  timestamp first_ending_timestamp = _get_first_timestamp(_downtime_ends);
  _downtimes[dwn.internal_id] = dwn;
  // Don't start already started downtimes.
  if (dwn.actual_start_time.is_null())
    _downtime_starts.insert(std::make_pair(start_time, dwn.internal_id));
  // Don't end already ended downtimes.
  if (dwn.actual_end_time.is_null())
    _downtime_ends.insert(std::make_pair(end_time, dwn.internal_id));

  // Wake thread.
  _general_condition.notify_all();
}

/**
 *  Remove a downtime.
 *
 *  @param[in] internal_id  The id of the downtime/
 */
void downtime_scheduler::remove_downtime(uint32_t internal_id) {
  // Lock the mutex.
  std::lock_guard<std::mutex> lock(_general_mutex);

  std::map<uint32_t, downtime>::iterator found = _downtimes.find(internal_id);
  if (found != _downtimes.end()) {
    for (std::multimap<timestamp, uint32_t>::iterator
             it = _downtime_starts.begin(),
             tmp = it, end = _downtime_starts.end();
         it != end; it = tmp) {
      ++tmp;
      if (it->second == internal_id)
        _downtime_starts.erase(it);
    }
    for (std::multimap<timestamp, uint32_t>::iterator
             it = _downtime_ends.begin(),
             tmp = it, end = _downtime_ends.end();
         it != end; it = tmp) {
      ++tmp;
      if (it->second == internal_id)
        _downtime_ends.erase(it);
    }
    _downtimes.erase(found);
  }
}

/**
 *  Get the first timestamp, or a null timestamp.
 *
 *  @return  The first timestamp, or a null timestamp.
 */
timestamp downtime_scheduler::_get_first_timestamp(
    std::multimap<timestamp, uint32_t> const& list) {
  return (list.begin() != list.end() ? list.begin()->first : timestamp());
}

/**
 *  @brief Called repeatedly by the downtime manager thread to process
 * downtimes.
 */
void downtime_scheduler::_process_downtimes() {
  timestamp now = ::time(nullptr);
  multiplexing::publisher pblsh;
  for (std::multimap<timestamp, uint32_t>::iterator
           it = _downtime_starts.begin(),
           tmp = it, end = _downtime_starts.end();
       it != end; it = tmp) {
    if (it->first > now)
      break;
    _start_downtime(_downtimes[it->second], &pblsh);
    ++tmp;
    _downtime_starts.erase(it);
  }
  for (std::multimap<timestamp, uint32_t>::iterator it = _downtime_ends.begin(),
                                                    tmp = it,
                                                    end = _downtime_ends.end();
       it != end; it = tmp) {
    if (it->first > now)
      break;
    _end_downtime(_downtimes[it->second], &pblsh);
    ++tmp;
    _downtimes.erase(_downtimes[it->second].internal_id);
    _downtime_ends.erase(it);
  }
}

/**
 *  Start a downtime.
 *
 *  @param[in] dwn     The downtime.
 *  @param[out] stream  The stream to write the downtime to.
 */
void downtime_scheduler::_start_downtime(downtime& dwn, io::stream* stream) {
  dwn.actual_start_time = ::time(nullptr);
  logging::debug(logging::medium)
      << "node events: starting downtime (" << dwn.start_time << "-"
      << dwn.end_time << ") on node (" << dwn.host_id << ", " << dwn.service_id
      << ") at " << dwn.actual_start_time;
  dwn.was_started = true;
  if (stream)
    stream->write(std::make_shared<downtime>(dwn));
}

/**
 *  End a downtime.
 *
 *  @param[in] dwn      The downtime.
 *  @param[out] stream  The stream to write the downtime to.
 */
void downtime_scheduler::_end_downtime(downtime& dwn, io::stream* stream) {
  dwn.actual_end_time = ::time(nullptr);
  logging::debug(logging::medium)
      << "node events: stopping downtime (" << dwn.start_time << "-"
      << dwn.end_time << ") on node (" << dwn.host_id << ", " << dwn.service_id
      << ") at " << dwn.actual_end_time;
  if (stream)
    stream->write(std::make_shared<downtime>(dwn));
}

void downtime_scheduler::wait() {
  _thread.join();
}
