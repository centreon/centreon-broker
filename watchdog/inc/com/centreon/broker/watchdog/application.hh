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

#ifndef CCB_WATCHDOG_APPLICATION_HH
#define CCB_WATCHDOG_APPLICATION_HH

#include <QEventLoop>
#include <QMap>
#include <QObject>
#include <QProcess>
#include <QSocketNotifier>
#include <QVector>
#include <memory>
#include <string>
#include "com/centreon/broker/logging/file.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/watchdog/configuration.hh"
#include "com/centreon/broker/watchdog/instance.hh"

namespace com {
namespace centreon {
namespace broker {
namespace watchdog {
/**
 *  @class application application.hh
 * "com/centreon/broker/watchdog/application.hh"
 *  @brief The application main loop.
 */
class application : public QEventLoop {
  Q_OBJECT

 public:
  application(std::string const& config_path);
  ~application();

  static int sighup_fd[2];
  static int sigterm_fd[2];

 public slots:
  void handle_sighup();
  void handle_sigterm();

 private:
  void _init();
  void _apply_new_configuration(configuration const& config);
  void _quit();

  // Configuration state.
  std::string _config_path;
  configuration _config;

  // Log backend.
  std::unique_ptr<com::centreon::broker::logging::file> _log;

  // Processes.
  std::map<std::string, instance*> _instances;

  // Notifier for signals.
  // We convert signals into write to a pipe, because signals can't be
  // meaningfully processed by QT otherwhise. This is the canon way
  // to handle signals in a QT application.
  std::unique_ptr<QSocketNotifier> _sighup;
  std::unique_ptr<QSocketNotifier> _sigterm;
};
}  // namespace watchdog
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_WATCHDOG_APPLICATION_HH
