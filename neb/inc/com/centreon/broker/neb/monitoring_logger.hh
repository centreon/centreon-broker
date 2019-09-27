/*
** Copyright 2012 Centreon
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

#ifndef CCB_NEB_MONITORING_LOGGER_HH
#define CCB_NEB_MONITORING_LOGGER_HH

#include "com/centreon/broker/logging/backend.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class monitoring_logger monitoring_logger.hh
 * "com/centreon/broker/neb/monitoring_logger.hh"
 *  @brief Log messages to the monitoring engine's log file.
 *
 *  Log messages to the monitoring engine's log file.
 */
class monitoring_logger : public logging::backend {
 public:
  monitoring_logger();
  monitoring_logger(monitoring_logger const& ml) = delete;
  ~monitoring_logger();
  monitoring_logger& operator=(monitoring_logger const& ml) = delete;
  void log_msg(char const* msg,
               unsigned int len,
               logging::type log_type,
               logging::level l) throw();

 private:
  void _internal_copy(monitoring_logger const& ml);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_MONITORING_LOGGER_HH
