/*
** Copyright 2011-2013,2015 Merethis
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
#include "com/centreon/broker/logging/logging.hh"
#include "test/processing/failover/setable_stream.hh"

using namespace com::centreon::broker;

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
 *  @param[in] other  Object to copy.
 */
setable_stream::setable_stream(setable_stream const& other)
  : com::centreon::broker::io::stream(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
setable_stream::~setable_stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
setable_stream& setable_stream::operator=(setable_stream const& other) {
  if (this != &other) {
    com::centreon::broker::io::stream::operator=(other);
    _internal_copy(other);
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
QList<misc::shared_ptr<io::data> > const& setable_stream::get_stored_events() const {
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
 *  @param[out] data      Some data.
 *  @param[in]  deadline  Unused.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool setable_stream::read(
                       misc::shared_ptr<io::data>& data,
                       time_t deadline) {
  (void)deadline;

  // Reset pointer.
  data.clear();

  // Sleep a while.
  QMutex m;
  QWaitCondition cv;
  cv.wait(&m, _sleep_time);

  // Do we generate an event ?
  if (_process_in && _process_out) {
    misc::shared_ptr<io::raw> raw(new io::raw);
    ++_count;
#if QT_VERSION >= 0x040500
    raw->append((char*)&_count, sizeof(_count));
#else
    raw->append(QByteArray((char*)&_count, sizeof(_count)));
#endif // Qt version.
    data = raw.staticCast<io::data>();
    logging::debug(logging::high) << "test: generating event #"
      << _count << " on test stream " << this;
  }
  // Provide retained event.
  else if (_replay_events && !_replay.isEmpty()) {
    data = _replay.front();
    _replay.pop_front();
    logging::debug(logging::high)
      << "test: reading replay event on test stream " << this;
  }
  // No processing possible.
  else
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "setable stream " << this << " is shutdown");
  return (true);
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
 *
 *  @return Number of events acknowledged (1).
 */
unsigned int setable_stream::write(misc::shared_ptr<io::data> const& d) {
  if (!_process_out)
    throw (io::exceptions::shutdown(!_process_in, !_process_out)
           << "setable stream " << this << " is shutdown");
  if (!d.isNull()) {
    if (_replay_events) {
      _replay.push_back(d);
      logging::debug(logging::high)
        << "test: storing replay event in test stream " << this
        << " which has now " << _replay.size() << " events";
    }
    if (_store_events) {
      _stored_events.push_back(d);
      logging::debug(logging::high)
        << "test: storing event in test stream " << this
        << " which has now " << _stored_events.size() << " events";
    }
  }
  return (1);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void setable_stream::_internal_copy(setable_stream const& other) {
  _count = other._count;
  _process_in = other._process_in;
  _process_out = other._process_out;
  _replay_events = other._replay_events;
  _replay = other._replay;
  _sleep_time = other._sleep_time;
  _store_events = other._store_events;
  _stored_events = other._stored_events;
  return ;
}
