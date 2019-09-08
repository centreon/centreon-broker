/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_PROCESS_HH
#define CCB_NOTIFICATION_PROCESS_HH

#include <QObject>
#include <QProcess>
#include <memory>
#include <string>
#include "com/centreon/broker/namespace.hh"

namespace com {
namespace centreon {
namespace broker {
namespace notification {
// Forward declaration.
class process_manager;
/**
 *  @class process process.hh "com/centreon/broker/notification/process.hh"
 *  @brief Represent a process being executed.
 */
class process : public QObject {
  Q_OBJECT

 public:
  process(int timeout = 0);

  unsigned int get_timeout() const throw();
  bool is_running() const;
  void kill();

  bool exec(std::string const& program, process_manager* manager = NULL);

  bool is_timeout() const throw();

  bool get_error(int& exit_code, std::string& error_output);

 public slots:
  void start(QString const& command_line);
  void error();
  void finished();
  void timeouted();

 signals:
  void finished(process&);
  void timeouted(process&);

 private:
  process(process const&);
  process& operator=(process const&);

  unsigned int _timeout;
  time_t _start_time;
  std::unique_ptr<QProcess> _process;

  bool _in_error;
  QProcess::ProcessError _error;
  QProcess::ExitStatus _status;
  int _exit_code;
  std::string _error_output;
};
}  // namespace notification
}  // namespace broker
}  // namespace centreon
}  // namespace com

#endif  // !CCB_NOTIFICATION_PROCESS_HH
