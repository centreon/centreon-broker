/*
** Copyright 2011-2013 Centreon
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

#include <QTimer>
#include <csignal>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/watchdog/instance.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

/**
 *  Default constructor.
 */
instance::instance(instance_configuration const& config)
  : _config(config) {
  if (config.should_run())
    start_instance();
}

/**
 *  Destructor.
 */
instance::~instance() {}

/**
 *  Start an instance broker.
 */
void instance::start_instance() {
  if (!_started) {
    _started = true;
    start(
      "cbd",
       QStringList(QString::fromStdString(_config.get_config_file())),
       QProcess::ReadOnly);
  }
}

/**
 *  Update an instance broker.
 */
void instance::update_instance() {
  if (state() == QProcess::Running)
    ::kill(pid(), SIGHUP);
}

/**
 *  Stop an instance broker.
 */
void instance::stop_instance() {
  _started = false;
  terminate();
  if (!waitForFinished()) {
    logging::error(logging::medium)
      << "watchdog: couldn't terminate properly the process '"
      << _config.get_name() << "'(" << pid() << "): killing it";
    kill();
  }
}

/**
 *  Called when the process exit.
 */
void instance::on_exit() {
  // Process should be quit, everything is going well.
  if (!_started || !_config.should_run())
    return;

  // Process should not be quit, restart it.
  logging::error(logging::medium)
    << "watchdog: process '" << _config.get_name()
    << "' has terminated unexpectedly, restarting it in "
    << _config.seconds_per_tentative() << " seconds";
  QTimer::singleShot(
    _config.seconds_per_tentative() * 1000,
    this,
    SLOT(start_instance()));
}
