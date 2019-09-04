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
#include <QTimer>
#include <csignal>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/vars.hh"
#include "com/centreon/broker/watchdog/application.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::watchdog;

/**
 *  Default constructor.
 */
instance::instance(instance_configuration const& config, application& parent)
    : QProcess(&parent), _config(config), _started(false) {
  if (config.should_run())
    start_instance();
  connect(this, SIGNAL(finished(int)), this, SLOT(on_exit()));
}

/**
 *  Destructor.
 */
instance::~instance() {
  stop_instance();
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

/**
 *  Start an instance of cbd.
 */
void instance::start_instance() {
  if (!_started && _config.should_run()) {
    _started = true;
    _since_last_start = timestamp::now();
    logging::info(logging::medium)
        << "watchdog: starting process '" << _config.get_name() << "'";
    start(PREFIX_BIN "/cbd",
          QStringList(QString::fromStdString(_config.get_config_file())),
          QProcess::ReadOnly);
    logging::info(logging::medium)
        << "watchdog: process '" << _config.get_name() << "' started (PID "
        << pid() << ")";
  }
  return;
}

/**
 *  Update an instance broker.
 */
void instance::update_instance() {
  if (state() == QProcess::Running && _config.should_reload()) {
    logging::info(logging::medium)
        << "watchdog: sending update signal to process '" << _config.get_name()
        << "' (PID " << pid() << ")";
    ::kill(pid(), SIGHUP);
  }
  return;
}

/**
 *  Stop an instance broker.
 */
void instance::stop_instance() {
  if (_started) {
    logging::info(logging::medium)
        << "watchdog: stopping process '" << _config.get_name() << "' (PID "
        << pid() << ")";
    _started = false;
    terminate();
    if (!waitForFinished(_exit_timeout)) {
      logging::error(logging::medium)
          << "watchdog: could npt gracefully terminate process '"
          << _config.get_name() << "' (PID " << pid() << "): killing it";
      kill();
    } else {
      logging::error(logging::medium)
          << "watchdog: process '" << _config.get_name()
          << "' stopped gracefully";
    }
  }
  return;
}

/**
 *  Called when the process exit.
 */
void instance::on_exit() {
  // Process should be stopped, everything is going well.
  if (!_started || !_config.should_run())
    return;

  _started = false;

  // Process should not be stopped, restart it.
  unsigned int time_to_restart =
      std::min(static_cast<unsigned int>(timestamp::now() - _since_last_start),
               _config.seconds_per_tentative());
  if (time_to_restart == 0)
    logging::error(logging::medium)
        << "watchdog: process '" << _config.get_name()
        << "' terminated unexpectedly, restarting it immediately";
  else
    logging::error(logging::medium)
        << "watchdog: process '" << _config.get_name()
        << "' terminated unexpectedly, restarting it in "
        << _config.seconds_per_tentative() << " seconds";
  QTimer::singleShot(_config.seconds_per_tentative() * 1000, this,
                     SLOT(start_instance()));
  return;
}
