/*
** Copyright 2011-2012 Merethis
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
#include "test/multiplexing/subscriber/temporary_stream.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
temporary_stream::temporary_stream() {

}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
temporary_stream::temporary_stream(temporary_stream const& ss)
  : io::stream(ss) {
  _events = ss._events;
}

/**
 *  Destructor.
 */
temporary_stream::~temporary_stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
temporary_stream& temporary_stream::operator=(temporary_stream const& ss) {
  if (this != &ss) {
    io::stream::operator=(ss);
    QMutexLocker lock1(&_eventsm);
    QMutexLocker lock2(&ss._eventsm);
    _events = ss._events;
  }
  return (*this);
}

/**
 *  Enable or disable event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Unused.
 */
void temporary_stream::process(bool in, bool out) {
  (void)in;
  (void)out;
  return ;
}

/**
 *  Read some data.
 *
 *  @param[out] data Some data.
 */
void temporary_stream::read(misc::shared_ptr<io::data>& data) {
  QMutexLocker lock(&_eventsm);
  if (_events.empty())
    data.clear();
  else
    data = _events.dequeue();
}

/**
 *  Write some data.
 *
 *  @param[in] d Data to write.
 */
void temporary_stream::write(misc::shared_ptr<io::data> const& d) {
  QMutexLocker lock(&_eventsm);
  _events.enqueue(d);
}
