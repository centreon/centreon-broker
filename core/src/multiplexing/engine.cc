/*
** Copyright 2009-2012 Merethis
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

#include <cassert>
#include <cstdlib>
#include <memory>
#include <QMutexLocker>
#include <QQueue>
#include <QVector>
#include <utility>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/internal.hh"
#include "com/centreon/broker/multiplexing/subscriber.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::multiplexing;

/**************************************
*                                     *
*            Local Objects            *
*                                     *
**************************************/

// Hooks.
static QVector<std::pair<hooker*, bool> >           _hooks;
static QVector<std::pair<hooker*, bool> >::iterator _hooks_begin;
static QVector<std::pair<hooker*, bool> >::iterator _hooks_end;

// Pointer.
std::auto_ptr<engine> engine::_instance;

// Data queue.
static QQueue<misc::shared_ptr<io::data> > _kiew;

// Mutex.
static QMutex _mutex(QMutex::Recursive);

// Processing flag.
static bool _processing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
engine::~engine() {}

/**
 *  Set a hook.
 *
 *  @param[in] h    Hook.
 *  @param[in] data Write data to hook.
 */
void engine::hook(hooker& h, bool data) {
  QMutexLocker lock(&_mutex);
  _hooks.push_back(std::make_pair(&h, data));
  _hooks_begin = _hooks.begin();
  _hooks_end = _hooks.end();
  return ;
}

/**
 *  Get engine instance.
 *
 *  @return Class instance.
 */
engine& engine::instance() {
  return (*_instance);
}

/**
 *  Load engine instance.
 */
void engine::load() {
  _instance.reset(new engine);
  return ;
}

/**
 *  Send an event to all subscribers.
 *
 *  @param[in] e Event to publish.
 */
void engine::publish(misc::shared_ptr<io::data> const& e) {
  // Lock mutex.
  QMutexLocker lock(&_mutex);

  // Store object for further processing.
  _kiew.enqueue(e);

  // Processing function.
  (this->*_write_func)(e);

  return ;
}

/**
 *  Start multiplexing.
 */
void engine::start() {
  if (_write_func != &engine::_write) {
    // Set writing method.
    logging::debug << logging::HIGH << "multiplexing: starting";
    _write_func = &engine::_write;

    // Copy event queue.
    QMutexLocker lock(&_mutex);
    QQueue<misc::shared_ptr<io::data> > kiew(_kiew);
    _kiew.clear();

    // Notify hooks of multiplexing loop start.
    for (QVector<std::pair<hooker*, bool> >::iterator
           it(_hooks_begin),
           end(_hooks_end);
         it != end;
         ++it) {
      it->first->starting();

      // Read events from hook.
      misc::shared_ptr<io::data> d;
      it->first->read(d);
      while (!d.isNull()) {
        _kiew.enqueue(d);
        it->first->read(d);
      }
    }

    // Process events from hooks.
    _send_to_subscribers();

    // Send events queued while multiplexing was stopped.
    while (!kiew.isEmpty()) {
      publish(kiew.head());
      kiew.dequeue();
    }
  }
  return ;
}

/**
 *  Stop multiplexing.
 */
void engine::stop() {
  if (_write_func != &engine::_nop) {
    // Notify hooks of multiplexing loop end.
    logging::debug(logging::high) << "multiplexing: stopping";
    QMutexLocker lock(&_mutex);
    for (QVector<std::pair<hooker*, bool> >::iterator
           it(_hooks_begin),
           end(_hooks_end);
         it != end;
         ++it) {
      it->first->stopping();

      // Read events from hook.
      misc::shared_ptr<io::data> d;
      it->first->read(d);
      while (!d.isNull()) {
        _kiew.enqueue(d);
        it->first->read(d);
      }
    }

    // Process events from hooks.
    _send_to_subscribers();

    // Set writing method.
    _write_func = &engine::_nop;
  }
  return ;
}

/**
 *  Remove a hook.
 *
 *  @param[in] h Hook.
 */
void engine::unhook(hooker& h) {
  QMutexLocker lock(&_mutex);
  for (QVector<std::pair<hooker*, bool> >::iterator
         it(_hooks_begin);
       it != _hooks.end();)
    if (it->first == &h)
      it = _hooks.erase(it);
    else
      ++it;
  _hooks_begin = _hooks.begin();
  _hooks_end = _hooks.end();
  return ;
}

/**
 *  Unload class instance.
 */
void engine::unload() {
  _instance.reset();
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
engine::engine() : _write_func(&engine::_nop) {
  // Initialize hook iterators.
  _hooks_begin = _hooks.begin();
  _hooks_end = _hooks.end();
}

/**
 *  @brief Copy constructor.
 *
 *  Any call to this constructor will result in a call to abort().
 *
 *  @param[in] e Unused.
 */
engine::engine(engine const& e) : QObject() {
  (void)e;
  assert(!"multiplexing engine is not copyable");
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] e Unused.
 *
 *  @return This object.
 */
engine& engine::operator=(engine const& p) {
  (void)p;
  assert(!"multiplexing engine is not copyable");
  abort();
  return (*this);
}

/**
 *  Do nothing.
 *
 *  @param[in] d Unused.
 */
void engine::_nop(misc::shared_ptr<io::data> const& d) {
  (void)d;
  return ;
}

/**
 *  On hook object destruction.
 *
 *  @param[in] obj Destroyed object.
 */
void engine::_on_hook_destroy(QObject* obj) {
  QMutexLocker lock(&_mutex);
  for (QVector<std::pair<hooker*, bool> >::iterator
         it = _hooks.begin();
       it != _hooks.end();)
    if (it->first == obj)
      it = _hooks.erase(it);
    else
      ++it;
  return ;
}

/**
 *  Send queued events to subscribers.
 */
void engine::_send_to_subscribers() {
  // Process all queued events.
  QMutexLocker lock(&gl_subscribersm);
  while (!_kiew.isEmpty()) {
    // Send object to every subscriber.
    for (QVector<subscriber*>::iterator
           it = gl_subscribers.begin(),
           end = gl_subscribers.end();
         it != end;
         ++it)
      (*it)->write(_kiew.head());
    _kiew.dequeue();
  }
  return ;
}

/**
 *  Publish event.
 *
 *  @param[in] d Data to publish.
 */
void engine::_write(misc::shared_ptr<io::data> const& e) {
  if (!_processing) {
    // Set processing flag.
    _processing = true;

    try {
      // Send object to every hook.
      for (QVector<std::pair<hooker*, bool> >::iterator
             it(_hooks_begin),
             end(_hooks_end);
           it != end;
           ++it)
        if (it->second) {
          it->first->write(e);
          misc::shared_ptr<io::data> d;
          it->first->read(d);
          while (!d.isNull()) {
            _kiew.enqueue(d);
            it->first->read(d);
          }
        }

      // Send events to subscribers.
      _send_to_subscribers();

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
