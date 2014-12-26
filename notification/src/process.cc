/*
** Copyright 2009-2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <QObject>
#include <QStringList>
#include "com/centreon/broker/notification/process.hh"
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
    _process(new QProcess) {}

/**
 *  Get the timeout of this process.
 *
 *  @return   The timeout of this process.
 */
unsigned int process::get_timeout() const throw() {
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
bool process::exec(
                std::string const& program,
                process_manager* manager /* = NULL */) {
  if (is_running())
    return (false);

  time(&_start_time);

  if (manager) {
    _process->moveToThread(&manager->get_thread());
    moveToThread(&manager->get_thread());
    QProcess::connect(
                this,
                SIGNAL(finished(process&)),
                manager,
                SIGNAL(process_finished(process&)));
    if (_timeout != 0) {
      QTimer* timer(new QTimer(this));
      timer->setSingleShot(true);
      connect(
        this,
        SIGNAL(timeouted(process&)),
        manager,
        SLOT(process_timeouted(process&)));
      connect(timer, SIGNAL(timeout()), this, SLOT(timeouted()));
    }
    QMetaObject::invokeMethod(
                   this,
                   "start",
                   Qt::QueuedConnection,
                   Q_ARG(QString, program.c_str()));
  }
  else {
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
  QProcess::connect(
             _process.get(),
             SIGNAL(QProcess::finished()),
             this,
             SLOT(finished()));
  QProcess::connect(
             _process.get(),
             SIGNAL(QProcess::error()),
             this,
             SLOT(finished()));
  _process->start(command_line);
  _process->closeWriteChannel();
}

/**
 *  The process was finished, or in an error state.
 */
void process::finished() {
  emit finished(*this);
}

/**
 *  The process timeouted.
 */
void process::timeouted() {
  emit timeouted(*this);
}
