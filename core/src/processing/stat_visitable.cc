/*
** Copyright 2011-2012,2017 Centreon
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

#include "com/centreon/broker/processing/stat_visitable.hh"

#include <unistd.h>

#include <map>

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/misc/string.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the thread.
 */
stat_visitable::stat_visitable(std::string const& name)
    : _state{""}, _queued_events{0}, _name(name) {}

/**
 *  Gather statistics on this thread.
 *
 *  @param[in] tree  Tree of information.
 */
void stat_visitable::stats(nlohmann::json& tree) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  tree["state"] = std::string(_state);
  tree["read_filters"] = _get_read_filters();
  tree["write_filters"] = _get_write_filters();
  tree["event_processing_speed"] =
      _event_processing_speed.get_processing_speed();
  tree["last_connection_attempt"] =
      static_cast<double>(_last_connection_attempt);
  tree["last_connection_success"] =
      static_cast<double>(_last_connection_success);
  tree["last_event_at"] =
      static_cast<double>(_event_processing_speed.get_last_event_time());
  tree["queued_events"] = static_cast<int>(_get_queued_events());

  // Forward the stats.
  _forward_statistic(tree);
}

/**
 *  Get this thread name.
 *
 *  @return  This thread name.
 */
std::string const& stat_visitable::get_name() const {
  return _name;
}

/**
 *  Set the last error.
 *
 *  @param[in] last_error  The last error.
 */
void stat_visitable::set_last_error(std::string const& last_error) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  _last_error = last_error;
}

/**
 *  Set last connection attempt.
 *
 *  @param[in] last_connection_attempt  Last connection attempt.
 */
void stat_visitable::set_last_connection_attempt(
    timestamp last_connection_attempt) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  _last_connection_attempt = last_connection_attempt;
}

/**
 *  Set last connection success.
 *
 *  @param[in] last_connection_success Last connection success.
 */
void stat_visitable::set_last_connection_success(
    timestamp last_connection_success) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  _last_connection_success = last_connection_success;
}

/**
 *  Tick the event processing computation.
 */
void stat_visitable::tick(uint32_t events) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  _event_processing_speed.tick(events);
}

/**
 *  @brief Delegate statistic to subojects.
 *
 *  Do nothing by default.
 *
 *  @param[in] tree  The tree gathering the stats.
 */
void stat_visitable::_forward_statistic(nlohmann::json& tree) {
  (void)tree;
}

void stat_visitable::set_state(char const* state) {
  _state = state;
}

void stat_visitable::set_queued_events(uint32_t queued_events) {
  _queued_events = queued_events;
}
