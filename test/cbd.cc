/*
** Copyright 2012-2013,2015 Centreon
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

#include "test/cbd.hh"
#include <csignal>
#include <cstdlib>
#include "test/vars.hh"

using namespace com::centreon::broker::test;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Default constructor.
 */
cbd::cbd() {}

/**
 *  Destructor.
 */
cbd::~cbd() {
  stop();
}

/**
 *  Set the configuration file.
 *
 *  @param[in] config_file Configuration file.
 */
void cbd::set_config_file(std::string const& config_file) {
  _config_file = config_file;
  return;
}

/**
 *  Start daemon.
 */
void cbd::start() {
  if (_cbd.state() == QProcess::NotRunning) {
    QStringList args;
    args.push_back(_config_file.c_str());
    _cbd.start(CBD_PATH, args);
    _cbd.waitForStarted();
  }
  return;
}

/**
 *  Stop Broker daemon.
 */
void cbd::stop() {
  if (_cbd.state() != QProcess::NotRunning) {
    _cbd.terminate();
    _cbd.waitForFinished(20000);
    if (_cbd.state() != QProcess::NotRunning) {
      _cbd.kill();
      _cbd.waitForFinished(-1);
    }
  }
  return;
}

/**
 *  Update the cbd daemon (SIGHUP).
 */
void cbd::update() {
  pid_t pid(_cbd.pid());
  if ((pid != (pid_t)0) && (pid != (pid_t)-1))
    kill(pid, SIGHUP);
  return;
}

/**
 *  Wait for process to terminate.
 */
void cbd::wait() {
  if (_cbd.state() != QProcess::NotRunning)
    _cbd.waitForFinished(-1);
  return;
}
