/*
** Copyright 2009-2012 Centreon
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

#ifndef CCB_LOGGING_LOGGER_HH
#define CCB_LOGGING_LOGGER_HH

#include "com/centreon/broker/logging/defines.hh"
#include "com/centreon/broker/logging/temp_logger.hh"

CCB_BEGIN()

namespace logging {
/**
 *  @class logger logger.hh "com/centreon/broker/logging/logger.hh"
 *  @brief Log messages.
 *
 *  Messages can be sent in various forms and stored in multiple
 *  facilities like syslog, files or standard output.
 */
class logger {
 public:
  logger(type log_type);
  ~logger();
  temp_logger operator()(level l) throw();

 private:
  logger(logger const& l);
  logger& operator=(logger const& l);

  type _type;
};
}  // namespace logging

CCB_END()

#endif  // !CCB_LOGGING_LOGGER_HH
