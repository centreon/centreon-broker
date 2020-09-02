/*
** Copyright 2015-2017 Centreon
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

#include "com/centreon/broker/watchdog/instance.hh"

#include <wait.h>

#include <csignal>

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

extern std::unique_ptr<spdlog::logger> logger;

/**
 *  Default constructor.
 */
instance::instance(instance_configuration const& config)
    : _config{config}, _started{false}, _pid{} {
  if (config.should_run())
    start();
}

/**
 *  Destructor.
 */
instance::~instance() {
  stop();
}

/**
 *  Merge a configuration with this instance.
 *
 *  @param[in] new_config  The new config.
 */
void instance::merge_configuration(instance_configuration const& new_config) {
  if (!_config.same_child(new_config)) {
    logger->error(
        "watchdog: Attempting to merge an incompatible configuration for "
        "process '{}': this is probably a software bug that should be reported "
        "to Centreon Broker developpers",
        _config.get_name());
    return;
  }
  _config = new_config;
}

/**
 *  A binding to the execve function.
 *
 * @param argv Args to give to the function.
 *
 * @return the pid.
 */
static pid_t exec_process(char const** argv) {
  pid_t son_pid{fork()};

  // I'm your father
  if (son_pid) {
  } else {
    int res = execve(argv[0], const_cast<char**>(argv), nullptr);
    exit(res);
  }
  return son_pid;
}

/**
 *  Restart an instance of cbd.
 */
void instance::restart() {
  _started = false;
  start();
}

/**
 *  Start an instance of cbd.
 */
void instance::start() {
  if (!_started && _config.should_run()) {
    _started = true;
    _since_last_start = timestamp::now();
    logger->info("watchdog: Starting progress '{}'", _config.get_name());
    char const* argv[]{_config.get_executable().c_str(),
                       _config.get_config_file().c_str(), nullptr};
    _pid = exec_process(argv);
    logger->info("watchdog: Process '{}' started (PID {})", _config.get_name(),
                 _pid);
  }
}

/**
 *  Update an instance broker.
 */
void instance::update() {
  if (_started && _config.should_reload()) {
    logger->info("watchdog: Sending update signal to process '{}' (PID {})",
                 _config.get_name(), _pid);
    kill(_pid, SIGHUP);
  }
}

/**
 *  Stop an instance broker.
 */
void instance::stop() {
  if (_started) {
    logger->info("watchdog: Stopping process '{}' (PID {})", _config.get_name(),
                 _pid);
    _started = false;
    int res = kill(_pid, SIGTERM);
    if (res)
      logger->error(
          "watchdog: Could not send a kill signal to process '{}' (PID {}): {}",
          _config.get_name(), _pid, strerror(errno));
    int status;
    int timeout = 15;
    while ((res = waitpid(_pid, &status, WNOHANG)) == 0) {
      if (--timeout < 0) {
        logger->error(
            "watchdog: Could not gracefully terminate process '{}' (PID {}): "
            "killing it",
            _config.get_name(), _pid);
        kill(_pid, SIGKILL);
        res = waitpid(_pid, &status, 0);
        if (res < 0)
          logger->error(
              "watchdog: Unable to kill the process '{}' (PID {}): {}",
              _config.get_name(), _pid, strerror(errno));
        else
          logger->info("watchdog: Process '{}' (PID {}) killed.",
                       _config.get_name(), _pid);
        return;
      }
      sleep(1);
    }
    if (res < 0)
      logger->error("watchdog: Unable to stop '{}' (PID {}): {}",
                    _config.get_name(), _pid, strerror(errno));
    else
      logger->info("watchdog: Process '{}' (PID {}) stopped gracefully",
                   _config.get_name(), _pid);
  }
}

/**
 *  Accessor to the pid.
 *
 * @return The pid.
 */
int instance::get_pid() const {
  return _pid;
}
