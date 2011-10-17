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

#include <QMutex>
#include <QWaitCondition>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "setable_stream.hh"

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] ss Object to copy.
 */
void setable_stream::_internal_copy(setable_stream const& ss) {
  _count = ss._count;
  _events = ss._events;
  _should_succeed = ss._should_succeed;
  _sleep_time = ss._sleep_time;
  _store_events = ss._store_events;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] should_succeed Success flag.
 */
setable_stream::setable_stream(QSharedPointer<volatile bool> ptr)
  : _count(0),
    _should_succeed(ptr),
    _sleep_time(0),
    _store_events(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] ss Object to copy.
 */
setable_stream::setable_stream(setable_stream const& ss)
  : com::centreon::broker::io::stream(ss) {
  _internal_copy(ss);
}

/**
 *  Destructor.
 */
setable_stream::~setable_stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ss Object to copy.
 *
 *  @return This object.
 */
setable_stream& setable_stream::operator=(setable_stream const& ss) {
  if (this != &ss) {
    com::centreon::broker::io::stream::operator=(ss);
    _internal_copy(ss);
  }
  return (*this);
}

/**
 *  Get current count value.
 *
 *  @return Count value.
 */
unsigned int setable_stream::count() const {
  return (_count);
}

/**
 *  Get retained events.
 *
 *  @return Events.
 */
QList<QSharedPointer<com::centreon::broker::io::data> > const& setable_stream::events() const {
  return (_events);
}

/**
 *  Read some data.
 *
 *  @return Some data.
 */
QSharedPointer<com::centreon::broker::io::data> setable_stream::read() {
  QMutex mx;
  QWaitCondition wc;
  mx.lock();
  wc.wait(&mx, _sleep_time);
  QSharedPointer<com::centreon::broker::io::raw> data;
  if (*_should_succeed) {
    data = QSharedPointer<com::centreon::broker::io::raw>(
      new com::centreon::broker::io::raw);
    ++_count;
    data->append((char*)&_count, sizeof(_count));
  }
  return (data.staticCast<com::centreon::broker::io::data>());
}

/**
 *  Set sleep time.
 *
 *  @param[in] ms Number of milliseconds to wait before reading an
 *                event.
 */
void setable_stream::set_sleep_time(unsigned int ms) {
  _sleep_time = ms;
  return ;
}

/**
 *  Set if stream should succeed or not.
 *
 *  @param[in] succeed true to make stream succeed.
 */
void setable_stream::set_succeed(bool succeed) {
  *_should_succeed = succeed;
  return ;
}

/**
 *  Set whether or not events should be stored when written.
 *
 *  @param[in] store true to store events.
 */
void setable_stream::store_events(bool store) {
  _store_events = store;
  return ;
}

/**
 *  Write some data.
 *
 *  @param[in] d Data to write.
 */
void setable_stream::write(QSharedPointer<com::centreon::broker::io::data> d) {
  if (!*_should_succeed)
    throw (com::centreon::broker::exceptions::msg() << "test: error");
  if (_store_events)
    _events.push_back(d);
  return ;
}
