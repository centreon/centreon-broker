/*
** Copyright 2012-2013 Merethis
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
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "test/multiplexing/subscriber/temporary_stream.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] id The temporary id.
 */
temporary_stream::temporary_stream(QString const& id) : _id(id) {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
temporary_stream::temporary_stream(temporary_stream const& ss)
  : io::stream(ss) {
  _events = ss._events;
  _id = ss._id;
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
    _id = ss._id;
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
    throw (io::exceptions::shutdown(false, false)
           << "temporary stream does not have any more event");
  else
    data = _events.dequeue();
  return ;
}

/**
 *  Write some data.
 *
 *  @param[in] d Data to write.
 *
 *  @return Number of elements acknowledged (1).
 */
unsigned int temporary_stream::write(misc::shared_ptr<io::data> const& d) {
  QMutexLocker lock(&_eventsm);
  _events.enqueue(d);
  return (1);
}
