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

#include <QStringList>
#include "com/centreon/broker/notification/process.hh"
#include "com/centreon/broker/notification/process_manager.hh"

#include "com/centreon/broker/logging/logging.hh"

using namespace com::centreon::broker::notification;

process_manager* process_manager::_instance_ptr = 0;

/**
 *  Get the singleton.
 *
 *  @return  The singleton.
 */
process_manager& process_manager::instance() {
  if (!_instance_ptr) {
    _instance_ptr = new process_manager;
    _instance_ptr->_thread->start();
  }
  return (*_instance_ptr);
}

/**
 *  Release (destroy and free) the singleton. Wait until the thread termination.
 */
void process_manager::release() {
  if (_instance_ptr) {
    _instance_ptr->_thread->exit(0);
    _instance_ptr->_thread->wait();
    delete _instance_ptr;
  }
}

/**
 *  @brief Create and execute a process.
 *
 *  Thread safe in and out the process manager thread.
 *
 *  @param[in] command  The command to be executed.
 *  @param[in] timeout  The timeout of the command.
 */
void process_manager::create_process(
                        std::string const& command,
                        unsigned int timeout) {
  // No memory leak possible because we set the parent of the process as us.
  process* pr(new process(timeout));

  {
    QMutexLocker lock(&_process_list_mutex);
    pr->setParent(this);
    _process_list.insert(pr);
  }

  pr->exec(command, this);
}

/**
 *  Default constructor.
 */
process_manager::process_manager()
  : _process_list_mutex(QMutex::Recursive) {
  _thread.reset(new QThread);
  moveToThread(_thread.get());
}

/**
 *  @brief A process was finished: reap the finished processes.
 *
 *  It is always executed from the QT event loop of the process manager thread.
 *
 *  @param[in] process  The process that has finished execution.
 */
void process_manager::process_finished(process& pr) {
  logging::debug(logging::medium)
    << "notification: a process has finished";

  std::string error_output;
  int exit_code;
  if (pr.get_error(exit_code, error_output))
    logging::error(logging::low)
      << "notification: error while executing a process: "
      << error_output;

  QMutexLocker lock(&_process_list_mutex);

  std::set<process*>::iterator found(_process_list.find(&pr));
  if (found != _process_list.end()) {
    delete *found;
    _process_list.erase(found);
  }
}

/**
 *  @brief A process was timeouted: reap the timeouted processes.
 *
 *  It is always executed from the QT event loop of the process manager thread.
 *
 *  @param[in] process  The process that has finished timeouted.
 */
void process_manager::process_timeouted(process& process) {
  logging::debug(logging::medium)
    << "notification: a process has timeouted";

  process.kill();
  process_finished(process);
}

/**
 *  Get the thread of this process manager.
 *
 *  @return  The thread of this process manager.
 */
QThread& process_manager::get_thread() {
  return (*_thread);
}
