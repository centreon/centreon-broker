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
#include <cassert>
#include <csignal>
#include <cstring>
#include <iostream>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/vars.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

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
    logging::error(logging::medium)
        << "watchdog: attempting to merge an incompatible configuration "
           "for process '"
        << _config.get_name()
        << "': this is probably a software bug that should be reported "
           "to Centreon Broker developpers";
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
  int status;

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
    logging::info(logging::medium)
        << "watchdog: starting process '" << _config.get_name() << "'";
    char const* argv[]{_config.get_executable().c_str(),
                       _config.get_config_file().c_str(), nullptr};
    _pid = exec_process(argv);
    logging::info(logging::medium)
        << "watchdog: process '" << _config.get_name() << "' started (PID "
        << _pid << ")";
  }
}

/**
 *  Update an instance broker.
 */
void instance::update() {
  if (_started && _config.should_reload()) {
    logging::info(logging::medium)
        << "watchdog: sending update signal to process '" << _config.get_name()
        << "' (PID " << _pid << ")";
    kill(_pid, SIGHUP);
  }
}

/**
 *  Stop an instance broker.
 */
void instance::stop() {
  if (_started) {
    logging::info(logging::medium)
        << "watchdog: stopping process '" << _config.get_name() << "' (PID "
        << _pid << ")";
    _started = false;
    int res = kill(_pid, SIGTERM);
    if (res)
      logging::error(logging::medium)
          << "watchdog: could not send a kill signal to process '"
          << _config.get_name() << "' (PID " << _pid << "):" << strerror(errno)
          << '\n';
    int status;
    int timeout = 10;
    while ((res = waitpid(_pid, &status, WNOHANG))) {
      if (--timeout < 0) {
        logging::error(logging::medium)
            << "watchdog: could not gracefully terminate process '"
            << _config.get_name() << "' (PID " << _pid << "): killing it";
        kill(_pid, SIGKILL);
        return;
      }
      sleep(1);
    }
    logging::error(logging::medium)
        << "watchdog: process '" << _config.get_name()
        << "' stopped gracefully";
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
