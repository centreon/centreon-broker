/*
** Copyright 2013,2015,2017 Centreon
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

#include "com/centreon/broker/misc/processing_speed_computer.hh"
#include <cstring>

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;

/**
 *  Default constructor.
 */
processing_speed_computer::processing_speed_computer()
    : _event_by_seconds(), _last_tick(timestamp::now()) {}

/**
 *  Get the event processing speed.
 *
 *  @return  The event processing speed.
 */
double processing_speed_computer::get_processing_speed() const {
  // If tick() was never called then no event was processed.
  if (_last_tick.is_null())
    return 0.0;

  // Compute event processing speed from the number of events processed
  // in /event_window_length/ seconds in the past. The most recent time
  // at which an event was computed is _last_tick. From then, no event
  // was processed.
  timestamp now(timestamp::now());
  int events(0);
  for (int i(0); i < window_length; ++i)
    events += _event_by_seconds[i];
  return static_cast<double>(events) / (window_length + now - _last_tick);
}

/**
 *  Register some number of events.
 *
 *  @param[in] events  The number of events to register.
 */
void processing_speed_computer::tick(int events) noexcept {
  // New second(s)
  timestamp now(timestamp::now());
  if (!_last_tick.is_null() && now > _last_tick) {
    int step(now - _last_tick);
    if (step < window_length && step > 0)
      ::memmove(_event_by_seconds.data() + step, _event_by_seconds.data(),
                (window_length - step) * sizeof(uint32_t));
    else
      step = window_length;
    ::memset(_event_by_seconds.data(), 0, step * sizeof(uint32_t));
  }

  // Update the data of this second.
  _event_by_seconds[0] += events;

  // Update the last tick.
  _last_tick = now;
}

/**
 *  Get the time of the last event.
 */
timestamp processing_speed_computer::get_last_event_time() const {
  return _last_tick;
}
