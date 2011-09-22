/*
** Copyright 2009-2011 Merethis
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

#include <algorithm>
#include <QMutexLocker>
#include <QQueue>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/internal.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

// Hooks.
static QList<QSharedPointer<hooker> > _hooks;

// Data queue.
static QQueue<QSharedPointer<io::data> > _kiew;

// Mutex.
static QMutex _mutex(QMutex::Recursive);

// Processing flag.
static bool _processing;

// Write method.
void (publisher::* publisher::_write_func)(QSharedPointer<io::data>) = &publisher::_nop;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Do nothing.
 *
 *  @param[in] d Unused.
 */
void publisher::_nop(QSharedPointer<io::data> d) {
  (void)d;
  return ;
}

/**
 *  Publish event.
 *
 *  @param[in] d Data to publish.
 */
void publisher::_write(QSharedPointer<io::data> e) {
  if (!_processing) {
    // Set processing flag.
    _processing = true;

    try {
      // Send object to every hook.
      for (QList<QSharedPointer<hooker> >::iterator it = _hooks.begin(),
             end = _hooks.end();
           it != end;
           ++it) {
        (*it)->write(e);
        QSharedPointer<io::data> d((*it)->read());
        while (!d.isNull()) {
          _kiew.enqueue(d);
          d = (*it)->read();
        }
      }

      // Process all queued events.
      while (!_kiew.isEmpty()) {
        // Send object to every subscriber.
        QMutexLocker lock(&gl_subscribersm);
        for (QList<subscriber*>::iterator it = gl_subscribers.begin(),
               end = gl_subscribers.end();
             it != end;
             ++it)
          (*it)->write(_kiew.head());
        _kiew.dequeue();
      }

      // Reset processing flag.
      _processing = false;
    }
    catch (...) {
      // Reset processing flag.
      _processing = false;
      throw ;
    }
  }

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
publisher::publisher() {}

/**
 *  Copy constructor.
 *
 *  @param[in] p Unused.
 */
publisher::publisher(publisher const& p) : io::stream(p) {}

/**
 *  Destructor.
 */
publisher::~publisher() {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Object to copy.
 *
 *  @return This object.
 */
publisher& publisher::operator=(publisher const& p) {
  io::stream::operator=(p);
  return (*this);
}

/**
 *  Set a hook.
 *
 *  @param[in] h Hook.
 */
void publisher::hook(QSharedPointer<hooker> h) {
  QMutexLocker lock(&_mutex);
  _hooks.push_back(h);
  return ;
}

/**
 *  Try to read.
 *
 *  @param[in] d Unused.
 */
QSharedPointer<io::data> publisher::read() {
  throw (exceptions::msg() << "multiplexing: tried to read from a " \
           "publisher");
  return (QSharedPointer<io::data>());
}

/**
 *  Start multiplexing.
 */
void publisher::start() {
  if (_write_func != &publisher::_write) {
    logging::debug << logging::HIGH << "multiplexing: starting";
    _write_func = &publisher::_write;
    for (QList<QSharedPointer<hooker> >::iterator
           it = _hooks.begin(),
           end = _hooks.end();
         it != end;
         ++it)
      (*it)->starting();
  }
  return ;
}

/**
 *  Stop multiplexing.
 */
void publisher::stop() {
  logging::debug << logging::HIGH << "multiplexing: stopping";
  for (QList<QSharedPointer<hooker> >::iterator
         it = _hooks.begin(),
         end = _hooks.end();
       it != end;
       ++it)
    (*it)->stopping();
  return ;
}

/**
 *  Remove a hook.
 *
 *  @param[in] h Hook.
 */
void publisher::unhook(QSharedPointer<hooker> h) {
  std::remove(_hooks.begin(), _hooks.end(), h);
  return ;
}

/**
 *  @brief Send an event to all subscribers.
 *
 *  As soon as the method returns, the event object is owned by the
 *  publisher, meaning that it'll be automatically destroyed when
 *  necessary.
 *
 *  @param[in] e Event to publish.
 */
void publisher::write(QSharedPointer<io::data> e) {
  // Lock mutex.
  QMutexLocker lock(&_mutex);

  // Store object for further processing.
  _kiew.enqueue(e);

  // Processing function.
  (this->*_write_func)(e);

  return ;
}
