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
#include <cstring>
#include <iostream>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/misc.hh"
#include "com/centreon/broker/vars.hh"
#include "com/centreon/broker/watchdog/application.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

/**
 *  Default constructor.
 */
instance::instance(instance_configuration const& config)
    : _config{config}, _started{false}, _pid{}, _fd{0} {
  if (config.should_run())
    start();
  // connect(this, SIGNAL(finished(int)), this, SLOT(on_exit()));
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
  if (_config != new_config) {
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

static std::pair<pid_t, int> exec_process(char const** argv) {
  int status;
  int fd[2];

  if (pipe(fd) == -1) {
    logging::error(logging::medium)
        << "watchdog: Unable to create the pipe to exchange with broker.";
    std::cout << "ERROR: unable to create the pipe to exchange with broker: "
              << strerror(errno) << '\n';
    exit(EXIT_FAILURE);
  }

  pid_t son_pid{fork()};

  // I'm your father
  if (son_pid) {
    // Let's close the write file descriptor.
    close(fd[1]);
  }
  else {
    // Let's close the read file descriptor.
    close(fd[0]);
    int res = execve(argv[0], const_cast<char**>(argv), nullptr);
    if (res == -1) {
      std::cout << "Child process failed: " << std::strerror(errno) << '\n';
    }
    write(fd[1], &res, sizeof(res));
    close(fd[1]);
    exit(res);
  }
  return {son_pid, fd[0]};
}

/**
 *  Restart an instance of cbd.
 */
void instance::restart() {
  _started = false;
  close(_fd);
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
    std::pair<pid_t, int> res{exec_process(argv)};
    _pid = res.first;
    _fd = res.second;
    logging::info(logging::medium)
        << "watchdog: process '" << _config.get_name() << "' started (PID "
        << _pid << ")";
  }
}

/**
 *  Update an instance broker.
 */
void instance::update_instance() {
  //  if (state() == QProcess::Running && _config.should_reload()) {
  //    logging::info(logging::medium)
  //        << "watchdog: sending update signal to process '" <<
  //        _config.get_name()
  //        << "' (PID " << pid() << ")";
  //    ::kill(pid(), SIGHUP);
  //  }
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
    close(_fd);
    kill(_pid, SIGTERM);
    int status;
    int timeout = 10;
    while (waitpid(_pid, &status, WNOHANG)) {
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
 *  Called when the process exit.
 */
void instance::on_exit() {
  //  // Process should be stopped, everything is going well.
  //  if (!_started || !_config.should_run())
  //    return;
  //
  //  _started = false;
  //
  //  // Process should not be stopped, restart it.
  //  unsigned int time_to_restart =
  //      std::min(static_cast<unsigned int>(timestamp::now() -
  //      _since_last_start),
  //               _config.seconds_per_tentative());
  //  if (time_to_restart == 0)
  //    logging::error(logging::medium)
  //        << "watchdog: process '" << _config.get_name()
  //        << "' terminated unexpectedly, restarting it immediately";
  //  else
  //    logging::error(logging::medium)
  //        << "watchdog: process '" << _config.get_name()
  //        << "' terminated unexpectedly, restarting it in "
  //        << _config.seconds_per_tentative() << " seconds";
  //  QTimer::singleShot(_config.seconds_per_tentative() * 1000, this,
  //                     SLOT(start()));
}

int instance::get_fd() const {
  return _fd;
}
