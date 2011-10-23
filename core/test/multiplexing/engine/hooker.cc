/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "test/multiplexing/engine/hooker.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
hooker::hooker() {}

/**
 *  Copy constructor.
 *
 *  @param[in] h Object to copy.
 */
hooker::hooker(hooker const& h)
  : multiplexing::hooker(h), _queue(h._queue) {}

/**
 *  Destructor.
 */
hooker::~hooker() {}

/**
 *  Assignment operator.
 *
 *  @param[in] h Object to copy.
 *
 *  @return This object.
 */
hooker& hooker::operator=(hooker const& h) {
  if (this != &h) {
    multiplexing::hooker::operator=(h);
    _queue = h._queue;
  }
  return (*this);
}

/**
 *  Read events from the hook.
 *
 *  @return Event.
 */
QSharedPointer<io::data> hooker::read() {
  QSharedPointer<io::data> d;
  if (!_queue.isEmpty()) {
    d = _queue.head();
    _queue.dequeue();
  }
  else if (!_registered)
    throw (io::exceptions::shutdown(true, true)
             << "hooker test object is shutdown");
  return (d);
}

/**
 *  Multiplexing engine is starting.
 */
void hooker::starting() {
  QSharedPointer<io::raw> raw(new io::raw);
  raw->append(HOOKMSG1);
  _queue.enqueue(raw);
  return ;
}

/**
 *  Multiplexing engine is stopping.
 */
void hooker::stopping() {
  QSharedPointer<io::raw> raw(new io::raw);
  raw->append(HOOKMSG3);
  _queue.enqueue(raw);
  return ;
}

/**
 *  Receive event to hook.
 *
 *  @param[in] d Ignored.
 */
void hooker::write(QSharedPointer<io::data> d) {
  (void)d;
  if (_registered) {
    QSharedPointer<io::raw> raw(new io::raw);
    raw->append(HOOKMSG2);
    _queue.enqueue(raw);
  }
  else
    throw (io::exceptions::shutdown(true, true)
             << "hooker test object is shutdown");
  return ;
}
