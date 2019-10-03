/*
** Copyright 2009-2014 Centreon
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

#include "com/centreon/broker/notification/process.hh"
#include <QObject>
#include <QStringList>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/process_manager.hh"

using namespace com::centreon::broker::notification;

/**
 *  Constructor.
 *
 *  @param[in] timeout  The timeout of this process, default 0 (none).
 */
process::process(int timeout /* = 0 */)
    : _timeout(timeout),
      _process(new QProcess),
      _in_error(false),
      _exit_code(-1) {}

/**
 *  Get the timeout of this process.
 *
 *  @return   The timeout of this process.
 */
uint32_t process::get_timeout() const throw() {
  return (_timeout);
}

/**
 *  Is this process running?
 *
 *  @return  True if this process is running.
 */
bool process::is_running() const {
  return (_process->state() == QProcess::Running);
}

/**
 *  Has this process timeouted?
 *
 *  @return  True if this process has timeouted.
 */
bool process::is_timeout() const throw() {
  return (_timeout > 0 ? difftime(time(NULL), _start_time) > _timeout : 0);
}

/**
 *  Kill this process.
 */
void process::kill() {
  if (is_running())
    _process->kill();
}

/**
 *  Get any error of the process.
 *
 *  @param[out] exit_code     The exit code of the process.
 *  @param[out] error_output  The standard error output of the process.
 *
 *  @return  True if an error ocurred.
 */
bool process::get_error(int& exit_code, std::string& error_output) {
  if (_in_error) {
    exit_code = _exit_code;
    error_output = _error_output;
    return (true);
  }
  return (false);
}

/**
 *  @brief Start this process with a command and an optional process manager.
 *
 *  If a process manager is given, the process manager will manage
 *  process timeout, process termination, and process freeing.
 *
 *  Else, the process will blocks until the end of its execution.
 *
 *  @param[in] program      The program to execute, with its arguments.
 *  @param[in,out] manager  The manager to which register the process.
 *
 *  @return                 True of the process was started.
 */
bool process::exec(std::string const& program,
                   process_manager* manager /* = NULL */) {
  if (is_running())
    return (false);

  time(&_start_time);

  if (manager) {
    _process->moveToThread(&manager->get_thread());
    moveToThread(&manager->get_thread());
    QProcess::connect(this, SIGNAL(finished(process&)), manager,
                      SLOT(process_finished(process&)));
    if (_timeout != 0) {
      QTimer* timer(new QTimer(this));
      timer->setSingleShot(true);
      connect(this, SIGNAL(timeouted(process&)), manager,
              SLOT(process_timeouted(process&)));
      connect(timer, SIGNAL(timeout()), this, SLOT(timeouted()));
    }
    QMetaObject::invokeMethod(this, "start", Qt::QueuedConnection,
                              Q_ARG(QString, program.c_str()));
  } else {
    return (_process->execute(program.c_str()) == 0);
  }
  return (true);
}

/**
 *  Start the process.
 *
 *  @param[in] command_line  The command to execute.
 */
void process::start(QString const& command_line) {
  QProcess::connect(_process.get(),
                    SIGNAL(QProcess::finished(int, QProcess::ExitStatus)), this,
                    SLOT(finished()));
  QProcess::connect(_process.get(),
                    SIGNAL(QProcess::error(QProcess::ProcessError)), this,
                    SLOT(error()));
  _process->start(command_line);
  _process->closeWriteChannel();
  _process->closeReadChannel(QProcess::StandardOutput);
}

/**
 *  The process is in an error state.
 */
void process::error() {
  _in_error = true;
  _error = _process->error();
}

/**
 *  The process was finished.
 */
void process::finished() {
  _exit_code = _process->exitCode();
  _status = _process->exitStatus();
  if (_exit_code != 0 || _status == QProcess::CrashExit)
    _in_error = true;
  _error_output = _process->readAllStandardError().data();
  emit finished(*this);
}

/**
 *  The process timeouted.
 */
void process::timeouted() {
  emit timeouted(*this);
}
