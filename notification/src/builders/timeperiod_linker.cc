/*
** Copyright 2011-2014 Centreon
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

#include "com/centreon/broker/notification/builders/timeperiod_linker.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::time;

/**
 *  Default constructor.
 */
timeperiod_linker::timeperiod_linker() {}

/**
 *  Add a timeperiod to the builder.
 *
 *  @param[in] id   The id of the timeperiod.
 *  @param[in] con  The timeperiod to add.
 */
void timeperiod_linker::add_timeperiod(uint32_t id, timeperiod::ptr con) {
  _table[id] = con;
}

/**
 *  Add a timeperiod exception to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] days           The days in the exception.
 *  @param[in] timerange      The timerange of the exception.
 */
void timeperiod_linker::add_timeperiod_exception(uint32_t timeperiod_id,
                                                 std::string const& days,
                                                 std::string const& timerange) {
  QHash<uint32_t, time::timeperiod::ptr>::iterator found =
      _table.find(timeperiod_id);
  if (found == _table.end())
    throw(exceptions::msg() << "timeperiod_linker: couldn't find timeperiod '"
                            << timeperiod_id << "'' for exception");
  (*found)->add_exception(days, timerange);
}

/**
 *  Add a timeperiod exclude relation to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] exclude_id     The id of the timeperiod excluded.
 */
void timeperiod_linker::add_timeperiod_exclude_relation(uint32_t timeperiod_id,
                                                        uint32_t exclude_id) {
  QHash<uint32_t, time::timeperiod::ptr>::iterator found =
      _table.find(timeperiod_id);
  QHash<uint32_t, time::timeperiod::ptr>::iterator excluded =
      _table.find(exclude_id);
  if (found == _table.end())
    throw(exceptions::msg() << "timeperiod_linker: couldn't find timeperiod '"
                            << timeperiod_id << "'' for exclusion");
  if (excluded == _table.end())
    throw(exceptions::msg() << "timeperiod_linker: couldn't find timeperiod '"
                            << exclude_id << "'' for excluded timeperiod");
  (*found)->add_excluded(*excluded);
}

/**
 *  Add a timeperiod include relation to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] include_id     The id of the timeperiod included.
 */
void timeperiod_linker::add_timeperiod_include_relation(uint32_t timeperiod_id,
                                                        uint32_t include_id) {
  QHash<uint32_t, time::timeperiod::ptr>::iterator found =
      _table.find(timeperiod_id);
  QHash<uint32_t, time::timeperiod::ptr>::iterator included =
      _table.find(include_id);
  if (found == _table.end())
    throw(exceptions::msg() << "timeperiod_linker: couldn't find timeperiod '"
                            << timeperiod_id << "'' for inclusion");
  if (included == _table.end())
    throw(exceptions::msg() << "timeperiod_linker: couldn't find timeperiod '"
                            << include_id << "'' for included timeperiod");
  (*found)->add_excluded(*included);
}
