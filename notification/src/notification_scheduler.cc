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

#include <QMutexLocker>
#include "com/centreon/broker/notification/notification_scheduler.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

notification_scheduler::notification_scheduler()
  : _should_exit(false) {}

void notification_scheduler::run() {
  _general_mutex.lock();
  while (1) {
    _general_condition.wait(&_general_mutex);
    if (_should_exit)
      break;

    // Processing.
  }
}

void notification_scheduler::exit() throw () {
  {
    QMutexLocker lock(&_general_mutex);
    _should_exit = true;
  }
  _general_condition.wakeAll();
}
