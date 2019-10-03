/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_LOGGING_MANAGER_HH
#define CCB_LOGGING_MANAGER_HH

#include <mutex>
#include <vector>
#include "com/centreon/broker/logging/backend.hh"
#include "com/centreon/broker/logging/defines.hh"
#include "com/centreon/broker/logging/temp_logger.hh"

namespace com {
namespace centreon {
namespace broker {
namespace logging {
/**
 *  @class manager manager.hh "com/centreon/broker/logging/manager.hh"
 *  @brief Logging object manager.
 *
 *  Register logging objects. This is the external access point
 *  to the logging system.
 */
class manager : public backend {
 private:
  struct manager_backend {
    backend* b;
    level l;
    uint32_t types;
  };
  std::vector<manager_backend> _backends;
  std::mutex _backendsm;
  static manager* _instance;
  uint32_t _limits[4];
  manager();
  manager(manager const& m);
  manager& operator=(manager const& m);
  void _compute_optimizations();

 public:
  ~manager();
  temp_logger get_temp_logger(type t, level l) throw();
  static manager& instance();
  static void load();
  void log_msg(char const* msg, uint32_t len, type t, level l) throw();
  void log_on(backend& b,
              uint32_t types = config_type | debug_type | error_type |
                                   info_type | perf_type,
              level min_priority = medium);
  static void unload();
};
}  // namespace logging
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_LOGGING_MANAGER_HH
