/*
** Copyright 2015 Centreon
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

#ifndef CCB_WATCHDOG_INSTANCE_HH
#define CCB_WATCHDOG_INSTANCE_HH

#include <unistd.h>

#include <string>

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/timestamp.hh"
#include "com/centreon/broker/watchdog/instance_configuration.hh"

namespace com {
namespace centreon {
namespace broker {
namespace watchdog {
class application;

/**
 *  @class instance instance.hh "com/centreon/broker/watchdog/instance.hh"
 *  @brief This represents the process associated with an instance of broker.
 */
class instance {
 public:
  instance(instance_configuration const& config);
  ~instance();

  void merge_configuration(instance_configuration const& new_config);

  void restart();
  void start();
  void stop();
  void update();

  void on_exit();
  int get_pid() const;

 private:
  instance(instance const&);
  instance& operator=(instance const&);

  instance_configuration _config;
  bool _started;
  timestamp _since_last_start;
  pid_t _pid;

  static const uint32_t _exit_timeout = 10000;
};
}  // namespace watchdog
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_WATCHDOG_INSTANCE_HH
