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

#ifndef CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/time/timeperiod.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class timeperiod_builder timeperiod_builder.hh
 * "com/centreon/broker/notification/builders/timeperiod_builder.hh"
 *  @brief Timeperiod builder interface.
 *
 *  This interface define what methods timeperiod builders need to implement.
 */
class timeperiod_builder {
 public:
  /**
   *  Virtual destructor.
   */
  virtual ~timeperiod_builder() {}

  /**
   *  Add a timeperiod to the builder.
   *
   *  @param[in] id The id of the timeperiod.
   *  @param[in] tperiod The timeperiod.
   */
  virtual void add_timeperiod(unsigned int id, time::timeperiod::ptr tperiod) {
    (void)id;
    (void)tperiod;
  }
  /**
   *  Add a timeperiod exception to the builder.
   *
   *  @param[in] timeperiod_id  The id of the timeperiod.
   *  @param[in] days           The days in the exception.
   *  @param[in] timerange      The timerange of the exception.
   */
  virtual void add_timeperiod_exception(unsigned int timeperiod_id,
                                        std::string const& days,
                                        std::string const& timerange) {
    (void)timeperiod_id;
    (void)days;
    (void)timerange;
  }

  /**
   *  Add a timeperiod exclude relation to the builder.
   *
   *  @param[in] timeperiod_id  The id of the timeperiod.
   *  @param[in] exclude_id     The id of the timeperiod excluded.
   */
  virtual void add_timeperiod_exclude_relation(unsigned int timeperiod_id,
                                               unsigned int exclude_id) {
    (void)timeperiod_id;
    (void)exclude_id;
  }

  /**
   *  Add a timeperiod include relation to the builder.
   *
   *  @param[in] timeperiod_id  The id of the timeperiod.
   *  @param[in] include_id     The id of the timeperiod included.
   */
  virtual void add_timeperiod_include_relation(unsigned int timeperiod_id,
                                               unsigned int include_id) {
    (void)timeperiod_id;
    (void)include_id;
  }
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_TIMEPERIOD_BUILDER_HH
