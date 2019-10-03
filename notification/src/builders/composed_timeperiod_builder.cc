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

#include "com/centreon/broker/notification/builders/composed_timeperiod_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::time;

/**
 *  Default constructor
 */
composed_timeperiod_builder::composed_timeperiod_builder() {}

/**
 *  Add a timeperiod to the builder.
 *
 *  @param[in] id The id of the timeperiod.
 *  @param[in] tperiod The timeperiod.
 */
void composed_timeperiod_builder::add_timeperiod(uint32_t id,
                                                 timeperiod::ptr tperiod) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_timeperiod(id, tperiod);
}

/**
 *  Add a timeperiod exception to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] days           The days in the exception.
 *  @param[in] timerange      The timerange of the exception.
 */
void composed_timeperiod_builder::add_timeperiod_exception(
    uint32_t timeperiod_id,
    std::string const& days,
    std::string const& timerange) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_timeperiod_exception(timeperiod_id, days, timerange);
}

/**
 *  Add a timeperiod exclude relation to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] exclude_id     The id of the timeperiod excluded.
 */
void composed_timeperiod_builder::add_timeperiod_exclude_relation(
    uint32_t timeperiod_id,
    uint32_t exclude_id) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_timeperiod_exclude_relation(timeperiod_id, exclude_id);
}

/**
 *  Add a timeperiod include relation to the builder.
 *
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] include_id     The id of the timeperiod included.
 */
void composed_timeperiod_builder::add_timeperiod_include_relation(
    uint32_t timeperiod_id,
    uint32_t include_id) {
  for (composed_builder<timeperiod_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_timeperiod_include_relation(timeperiod_id, include_id);
}
