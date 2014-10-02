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

process_manager* process_manager::instance() {
  if (!_instance_ptr) {
    _instance_ptr = new process_manager;
    _instance_ptr->start();
  }
  return (_instance_ptr);
}

void process_manager::release() {
  if (_instance_ptr) {
    if (_instance_ptr->isRunning()) {
      _instance_ptr->exit(0);
      _instance_ptr->wait();
    }
    delete _instance_ptr;
  }
}

void process_manager::run() {
  exec();
}

void process_manager::create_process(std::string const& command,
                                     std::list<std::string> const& args,
                                     unsigned int timeout) {
  process* pr = new process(timeout);

  {
    QMutexLocker lock(&_process_list_mutex);
    _process_list.push_back(misc::shared_ptr<process>(pr));
  }

  pr->exec(command, args, this);
}

process_manager::process_manager()
  : _process_list_mutex(QMutex::Recursive) {}

void process_manager::process_finished() {
  QMutexLocker lock(&_process_list_mutex);

  for (std::list<misc::shared_ptr<process> >::iterator it(_process_list.begin()),
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
