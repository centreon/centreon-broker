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

#include <map>
#include <unistd.h>
#include <QMutexLocker>
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/processing/stat_visitable.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::processing;

/**
 *  Constructor.
 *
 *  @param[in] name  The name of the thread.
 */
stat_visitable::stat_visitable(std::string const& name)
  : _name(name) {}

/**
 *  Destructor.
 */
stat_visitable::~stat_visitable() {}

/**
 *  Dump all the filters in a string.
 *
 *  @param[in] filters  The filters.
 *
 *  @return             A string containing all the filters.
 */
static std::string dump_filters(uset<unsigned int> const& filters) {
  io::events::events_container all_event_container
    = io::events::instance().get_events_by_category_name("all");
  std::map<unsigned int, std::string> name_by_id;

  uset<unsigned int> all_events;
  for (io::events::events_container::const_iterator
         it = all_event_container.begin(),
         end = all_event_container.end();
       it != end;
       ++it) {
    all_events.insert(it->first);
    name_by_id[it->first] = it->second.get_name();
  }

  if (filters.size() == all_events.size())
    return ("all");

  std::string ret;
  for (uset<unsigned int>::const_iterator
         it = filters.begin(),
         end = filters.end();
       it != end;
       ++it) {
     std::map<unsigned int, std::string>::const_iterator
       found = name_by_id.find(*it);
     if (found != name_by_id.end())
       ret.append(",  ").append(found->second);
  }
  return (ret);
}

/**
 *  Gather statistics on this thread.
 *
 *  @param[in] tree  Tree of information.
 */
void stat_visitable::stats(io::properties &tree) {
  std::lock_guard<std::mutex> lock(_stat_mutex);
  tree.add_property("state", io::property("state", _get_state()));
  tree.add_property(
         "read_filters",
         io::property(
               "read_filters",
               dump_filters(_get_read_filters())));
  tree.add_property(
         "write_filters",
         io::property(
               "write_filters",
               dump_filters(_get_write_filters())));
  tree.add_property(
         "event_processing_speed",
         io::property(
               "event_processing_speed",
               misc::string::get(
                       _event_processing_speed.get_processing_speed())));
  tree.add_property(
         "last_connection_attempt",
         io::property(
           "last_connection_attempt",
           misc::string::get(_last_connection_attempt)));
  tree.add_property(
         "last_connection_success",
         io::property(
           "last_connection_success",
           misc::string::get(_last_connection_success)));
  tree.add_property(
         "last_event_at",
         io::property(
              "last_event_at",
               misc::string::get(
                       _event_processing_speed.get_last_event_time())));
  tree.add_property(
         "queued_events",
         io::property(
               "queued_events",
               misc::string::get(_get_queued_events())));

  // Forward the stats.
  _forward_statistic(tree);
}

/**
 *  Get this thread name.
 *
 *  @return  This thread name.
 */
std::string const& stat_visitable::get_name() const {
  return (_name);
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
void stat_visitable::tick(unsigned int events) {
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
void stat_visitable::_forward_statistic(io::properties& tree) {
  (void)tree;
}
