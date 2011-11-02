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
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/io/raw.hh"
#include "test/processing/feeder/setable_stream.hh"

using namespace com::centreon::broker;

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
  _process_in = ss._process_in;
  _process_out = ss._process_out;
  _replay_events = ss._replay_events;
  _replay = ss._replay;
  _sleep_time = ss._sleep_time;
  _store_events = ss._store_events;
  _stored_events = ss._stored_events;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
setable_stream::setable_stream()
  : _count(0),
    _process_in(true),
    _process_out(true),
    _replay_events(false),
    _sleep_time(10),
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
unsigned int setable_stream::get_count() const {
  return (_count);
}

/**
 *  Get retained events.
 *
 *  @return Events.
 */
QList<QSharedPointer<io::data> > const& setable_stream::get_stored_events() const {
  return (_stored_events);
}

/**
 *  Enable or disable event processing.
 *
 *  @param[in] in  Set to true to enable input event processing.
 *  @param[in] out Set to true to enable output event processing.
 */
void setable_stream::process(bool in, bool out) {
  _process_in = in;
  _process_out = out;
  return ;
}

/**
 *  Read some data.
 *
 *  @return Some data.
 */
QSharedPointer<io::data> setable_stream::read() {
  // Sleep a while.
  QMutex m;
  QWaitCondition cv;
  cv.wait(&m, _sleep_time);

  // Do we generate an event ?
  QSharedPointer<io::data> data;
  if (_process_in && _process_out) {
    QSharedPointer<io::raw> raw(new io::raw);
    ++_count;
    raw->append((char*)&_count, sizeof(_count));
    data = raw.staticCast<io::data>();
  }
  // Provide retained event.
  else if (_replay_events && !_replay.isEmpty()) {
    data = _replay.front();
    _replay.pop_front();
  }
  // No processing possible.
  else
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "setable stream is shutdown");
  return (data);
}

/**
 *  Set current count value.
 *
 *  @param[in] cnt New count value.
 */
void setable_stream::set_count(unsigned int cnt) {
  _count = cnt;
  return ;
}

/**
 *  Set replay events.
 *
 *  @param[in] replay Set to true to replay events that have been
 *                    written to this stream.
 */
void setable_stream::set_replay_events(bool replay) {
  _replay_events = replay;
  return ;
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
 *  Set whether or not events should be stored when written.
 *
 *  @param[in] store true to store events.
 */
void setable_stream::set_store_events(bool store) {
  _store_events = store;
  return ;
}

/**
 *  Write some data.
 *
 *  @param[in] d Data to write.
 */
void setable_stream::write(QSharedPointer<io::data> d) {
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
             << "setable stream is shutdown");
  if (_replay_events)
    _replay.push_back(d);
  if (_store_events)
    _stored_events.push_back(d);
  return ;
}
