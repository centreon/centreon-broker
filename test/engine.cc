/*
** Copyright 2012,2015 Centreon
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

#include <csignal>
#include <cstdlib>
#include "test/engine.hh"
#include "test/vars.hh"

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
engine::engine() {}

/**
 *  Destructor.
 */
engine::~engine() {
  stop();
}

/**
 *  Reload Engine.
 */
void engine::reload() {
  pid_t pid(_engine.pid());
  if ((pid != (pid_t)0) && (pid != (pid_t)-1))
    kill(pid, SIGHUP);
  return ;
}

/**
 *  Set the configuration file.
 *
 *  @param[in] config_file Configuration file.
 */
void engine::set_config_file(std::string const& config_file) {
  _config_file = config_file;
  return ;
}

/**
 *  Start monitoring engine.
 */
void engine::start() {
  if (_engine.state() == QProcess::NotRunning) {
    QStringList args;
    args.push_back(_config_file.c_str());
    _engine.start(MONITORING_ENGINE, args);
    _engine.waitForStarted();
  }
  return ;
}

/**
 *  Stop monitoring engine.
 */
void engine::stop() {
  if (_engine.state() != QProcess::NotRunning) {
    _engine.terminate();
    _engine.waitForFinished(20000);
    if (_engine.state() != QProcess::NotRunning) {
      _engine.kill();
      _engine.waitForFinished(-1);
    }
  }
  return ;
}
