/*
** Copyright 2009-2012,2017 Centreon
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

#ifndef CCB_LOGGING_DEFINES_HH
#define CCB_LOGGING_DEFINES_HH

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace logging {
// Log levels.
enum level { none = 0, high, medium, low };

// Timestamp types.
enum timestamp_type { no_timestamp = 0, second_timestamp, nano_timestamp };

// Log types.
enum type {
  config_type = 1,
  debug_type = 2,
  error_type = 4,
  info_type = 8,
  perf_type = 16
};
}  // namespace logging

CCB_END()

#endif  // !CCB_LOGGING_DEFINES_HH
