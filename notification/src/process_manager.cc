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
    _instance_ptr->start();
  }
  return (*_instance_ptr);
}

/**
 *  Release (destroy and free) the singleton. Wait until the thread termination.
 */
void process_manager::release() {
  if (_instance_ptr) {
    if (_instance_ptr->isRunning()) {
      _instance_ptr->exit(0);
      _instance_ptr->wait();
    }
    delete _instance_ptr;
  }
}

/**
 *  Called when the thread start. Run the event loop.
 */
void process_manager::run() {
  exec();
}

/**
 *  @brief Create and execute a process.
 *
 *  Thread safe in and out the process manager thread.
 *
 *  @param[in] command  The command to be executed.
 *  @param[in] timeout  The timeout of the command.
 */
void process_manager::create_process(std::string const& command,
                                     unsigned int timeout) {
  process* pr = new process(timeout);

  {
    QMutexLocker lock(&_process_list_mutex);
    _process_list.push_back(misc::shared_ptr<process>(pr));
  }

  pr->exec(command, this);
}

/**
 *  Default constructor.
 */
process_manager::process_manager()
  : _process_list_mutex(QMutex::Recursive) {moveToThread(this);}

/**
 *  @brief A process was finished: reap the finished processes.
 *
 *  It is always executed from the QT event loop of the process manager thread.
 */
void process_manager::process_finished() {
  QMutexLocker lock(&_process_list_mutex);

  for (std::list<misc::shared_ptr<process> >::iterator
                                                it(_process_list.begin()),
                                                end(_process_list.end());
       ++it != end;) {
    if (!(*it)->is_running()) {
      // Do something with the process.

      // Remove the process.
      std::list<misc::shared_ptr<process> >::iterator tmp = it;
      ++it;
      _process_list.erase(tmp);
    }
    else
      ++it;
  }
}

/**
 *  @brief A process was timeouted: reap the timeouted processes.
 *
 *  It is always executed from the QT event loop of the process manager thread.
 */
void process_manager::process_timeouted() {
  QMutexLocker lock(&_process_list_mutex);

  for (std::list<misc::shared_ptr<process> >::iterator
                                                it(_process_list.begin()),
                                                end(_process_list.end());
       it != end;) {
    if ((*it)->is_timeout()) {
      // Kill the process.
      (*it)->kill();
      // Remove the process.
      std::list<misc::shared_ptr<process> >::iterator tmp = it;
      ++it;
      _process_list.erase(tmp);
    }
    else
      ++it;
  }

  for (std::list<misc::shared_ptr<QTimer> >::iterator it(_timer_list.begin()),
                                                      end(_timer_list.end());
       it != end;) {
    if (!(*it)->isActive()) {
      std::list<misc::shared_ptr<QTimer> >::iterator tmp = it;
      ++it;
      _timer_list.erase(tmp);
    }
    else
      ++it;
  }
}

/**
 *  @brief Add a timeout.
 *
 *  Thread safe in and out the of the process manager thread.
 *
 *  @param[in] timeout  The timeout to add, in second.
 */
void process_manager::add_timeout(unsigned int timeout) {
  QMutexLocker lock(&_process_list_mutex);

  QTimer* timer = new QTimer(this);
  timer->moveToThread(this);
  timer->setSingleShot(true);
  connect(timer, SIGNAL(timeout()), this, SLOT(process_timeouted()));
  timer->start(timeout * 1000);
  _timer_list.push_back(misc::shared_ptr<QTimer>(timer));
}
