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

#ifndef CCB_BAM_DIMENSION_TIMEPERIOD_EXCEPTION_HH
#define CCB_BAM_DIMENSION_TIMEPERIOD_EXCEPTION_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace bam {
/**
 *  @class dimension_timeperiod_exception dimension_timeperiod_exception.hh
 * "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
 *  @brief Timeperiod exception.
 *
 *  Declare a timeperiod exception.
 */
class dimension_timeperiod_exception : public io::data {
 public:
  dimension_timeperiod_exception();
  dimension_timeperiod_exception(dimension_timeperiod_exception const& other);
  ~dimension_timeperiod_exception();
  dimension_timeperiod_exception& operator=(
      dimension_timeperiod_exception const& other);
  uint32_t type() const;
  static uint32_t static_type();

  std::string daterange;
  std::string timerange;
  uint32_t timeperiod_id;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(dimension_timeperiod_exception const& other);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_DIMENSION_TIMEPERIOD_EXCEPTION_HH
