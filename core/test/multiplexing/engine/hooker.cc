/*
** Copyright 2011-2013 Merethis
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
 *  @param[out] d Event.
 */
void hooker::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  if (!_queue.isEmpty()) {
    d = _queue.head();
    _queue.dequeue();
  }
  else if (!_registered)
    throw (io::exceptions::shutdown(true, true)
             << "hooker test object is shutdown");
  return ;
}

/**
 *  Multiplexing engine is starting.
 */
void hooker::starting() {
  misc::shared_ptr<io::raw> raw(new io::raw);
  raw->append(HOOKMSG1);
  _queue.enqueue(raw.staticCast<io::data>());
  return ;
}

/**
 *  Multiplexing engine is stopping.
 */
void hooker::stopping() {
  misc::shared_ptr<io::raw> raw(new io::raw);
  raw->append(HOOKMSG3);
  _queue.enqueue(raw.staticCast<io::data>());
  return ;
}

/**
 *  Receive event to hook.
 *
 *  @param[in] d Ignored.
 *
 *  @return Number of elements acknowledged (1).
 */
unsigned int hooker::write(misc::shared_ptr<io::data> const& d) {
  (void)d;
  if (_registered) {
    misc::shared_ptr<io::raw> raw(new io::raw);
    raw->append(HOOKMSG2);
    _queue.enqueue(raw.staticCast<io::data>());
  }
  else
    throw (io::exceptions::shutdown(true, true)
           << "hooker test object is shutdown");
  return (1);
}
