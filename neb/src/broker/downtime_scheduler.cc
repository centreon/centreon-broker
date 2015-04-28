/*
** Copyright 2014 Merethis
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

#include <ctime>
#include <limits>
#include <QMutexLocker>
#include "com/centreon/broker/neb/downtime_scheduler.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
downtime_scheduler::downtime_scheduler()
  : _should_exit(false),
    _general_mutex(QMutex::NonRecursive) {}

/**
 *  Called by the downtime manager thread when it starts.
 */
void downtime_scheduler::run() {
  bool just_started = true;

  while (1) {
    // Lock the general mutex used by the notification scheduler.
    _general_mutex.lock();

    // Signal the thread waiting on us that we have started.
    if (just_started) {
      _started.release();
      just_started = false;
    }

    // Wait until the first downtime in the queue - or forever until awakened
    // if none.
    time_t first_time = std::min(
                          _get_first_timestamp(_downtime_starts),
                          _get_first_timestamp(_downtime_ends));
    time_t now = ::time(NULL);
    unsigned long wait_for = first_time == time_t(-1) ?
                               std::numeric_limits<unsigned long>::max()
                               : (first_time >= now) ?
                                   (first_time - now) * 1000
                                   : 0;

    logging::debug(logging::medium)
      << "neb: downtime scheduler sleeping for "
      << wait_for / 1000.0 << " seconds";

    _general_condition.wait(&_general_mutex, wait_for);

    logging::debug(logging::medium)
        << "neb: downtime scheduler waking up";

    // The should exit flag was set - exit.
    if (_should_exit)
      break ;

    // Process the downtimes and release the mutex.
    _process_downtimes();
  }
}

/**
 *  Start the downtime scheduler and wait until it has started.
 */
void downtime_scheduler::start_and_wait() {
  QThread::start();  
  // Wait until the thread has started.
  _started.acquire();
}

/**
 *  Ask gracefully for the downtime scheduling thread to exit.
 */
void downtime_scheduler::exit() throw () {
  // Set the should exit flag.
  {
    QMutexLocker lock(&_general_mutex);
    _should_exit = true;
    // Wake the notification scheduling thread.
    _general_condition.wakeAll();
  }
}

/**
 *  Add a downtime to the list of managed downtimes.
 *
 *  @param[in] dwn       The downtime.
 */
void downtime_scheduler::add_downtime(
                           downtime const& dwn) {
  if (dwn.start_time >= dwn.end_time) {
    logging::debug(logging::medium)
      << "neb: attempt to schedule a downtime when start time "
         "is superior or equal to its end time";
    return ;
  }

  // Lock the mutex.
  QMutexLocker lock(&_general_mutex);

  timestamp first_starting_timestamp = _get_first_timestamp(_downtime_starts);
  timestamp first_ending_timestamp = _get_first_timestamp(_downtime_ends);
  _downtimes[dwn.internal_id] = dwn;
  _downtime_starts.insert(std::make_pair(dwn.start_time, dwn.internal_id));
  _downtime_ends.insert(std::make_pair(dwn.end_time, dwn.internal_id));

  // If we just added a timestamp < the previous first timestamps,
  // wake the thread up.
  if (_get_first_timestamp(_downtime_starts) != first_starting_timestamp
        || _get_first_timestamp(_downtime_ends) != first_ending_timestamp)
    _general_condition.wakeAll();
}

/**
 *  Get the first timestamp, or a null timestamp.
 *
 *  @return  The first timestamp, or a null timestamp.
 */
timestamp downtime_scheduler::_get_first_timestamp(
            std::multimap<timestamp, unsigned int> const& list) {
  return (list.begin() != list.end() ?
            list.begin()->first :
            timestamp());
}

/**
 *  @brief Called repeatedly by the downtime manager thread to process downtimes.
 */
void downtime_scheduler::_process_downtimes() {
  timestamp now = ::time(NULL);
  multiplexing::publisher pblsh;
  for (std::multimap<timestamp, unsigned int>::iterator
         it = _downtime_starts.begin(),
         tmp = it,
         end = _downtime_starts.end();
       it != end;
       it = tmp) {
    if (it->first > now)
      break;
    _start_downtime(_downtimes[it->second], &pblsh);
    ++tmp;
    _downtime_starts.erase(it);
  }
  for (std::multimap<timestamp, unsigned int>::iterator
         it = _downtime_ends.begin(),
         tmp = it,
         end = _downtime_ends.end();
       it != end;
       it = tmp) {
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
  dwn.actual_start_time = ::time(NULL);
  if (stream)
    stream->write(misc::make_shared(new downtime(dwn)));
}

/**
 *  End a downtime.
 *
 *  @param[in] dwn      The downtime.
 *  @param[out] stream  The stream to write the downtime to.
 */
void downtime_scheduler::_end_downtime(downtime& dwn, io::stream* stream) {
  dwn.actual_end_time = ::time(NULL);
  if (stream)
    stream->write(misc::make_shared(new downtime(dwn)));
}
