/*
** Copyright 2009-2011 Merethis
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

#include <assert.h>
#include <QReadLocker>
#include <QWriteLocker>
#include <stdlib.h>
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker::logging;

/**************************************
*                                     *
*           Private Objects           *
*                                     *
**************************************/

// Class instance.
QScopedPointer<manager> manager::_instance;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
manager::manager() : _level(none), _types(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] m Unused.
 */
manager::manager(manager const& m) : QObject() {
  (void)m;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  Any call to this method will result in a call to abort().
 *
 *  @param[in] m Unused.
 *
 *  @return This object.
 */
manager& manager::operator=(manager const& m) {
  (void)m;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Compute optimizations to avoid as much log message generation as
 *  possible.
 */
void manager::_compute_optimizations() {
  _level = none;
  _types = 0;
  for (QHash<backend*, QPair<unsigned int, level> >::const_iterator
         it = _backends.begin(),
         end = _backends.end();
       it != end;
       ++it) {
    _types |= it->first;
    if (it->second > _level)
      _level = it->second;
  }
  return ;
}

/**
 *  @brief Receive backend destruction notifications.
 *
 *  On receive of such notifications, this method will update
 *  optimization parameters to avoid generating as much log messages as
 *  possible.
 *
 *  @param[in] obj Backend pointer.
 */
void manager::_on_backend_destruction(QObject* obj) {
  QWriteLocker lock(&_backendsm);

  // Remove backend from backend list.
  _backends.remove((backend*)obj);

  // Recompute optimization parameters.
  _compute_optimizations();

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
manager::~manager() {}

/**
 *  Get temporary logging object.
 *
 *  @param[in] t Log type.
 *  @param[in] l Log level.
 *
 *  @return Temporary logging object.
 */
temp_logger manager::get_temp_logger(type t, level l) throw () {
  return (temp_logger(t, l, (_types & t) && (l <= _level)));
}

/**
 *  Get class instance.
 *
 *  @return Class instance.
 */
manager& manager::instance() {
  return (*_instance);
}

/**
 *  Load the manager singleton.
 */
void manager::load() {
  _instance.reset(new manager);
  return ;
}

/**
 *  Log message to backends.
 *
 *  @param[in] msg Message to log.
 *  @param[in] len Message length.
 *  @param[in] t   Log type.
 *  @param[in] l   Log level.
 */
void manager::log_msg(char const* msg,
                      unsigned int len,
                      type t,
                      level l) throw () {
  QReadLocker lock(&_backendsm);
  for (QHash<backend*, QPair<unsigned int, level> >::iterator
         it = _backends.begin(),
         end = _backends.end();
       it != end;
       ++it)
    if (msg && (it->first & t) && (it->second >= l)) {
      QMutexLocker lock(it.key());
      it.key()->log_msg(msg, len, t, l);
    }
  return ;
}

/**
 *  @brief Add or remove a log backend.
 *
 *  If either types or min_priority is 0, the backend will be removed.
 *
 *  @param[in] b            Backend.
 *  @param[in] types        Log types to store on this backend. Bitwise
 *                          OR of multiple logging::type.
 *  @param[in] min_priority Minimal priority of messages to be logged.
 */
void manager::log_on(backend& b,
                     unsigned int types,
                     level min_priority) {
  QWriteLocker lock(&_backendsm);

  // Either add backend to list.
  if (types && min_priority) {
    QPair<unsigned int, level>& p(_backends[&b]);
    p.first = types;
    p.second = min_priority;
    _types |= types;
    if (min_priority > _level)
      _level = min_priority;
    connect(
      &b,
      SIGNAL(destroyed(QObject*)),
      this,
      SLOT(_on_backend_destruction(QObject*)));
  }
  // Or remove it.
  else {
    _backends.remove(&b);
    _compute_optimizations();
  }

  return ;
}

/**
 *  Unload the logging manager.
 */
void manager::unload() {
  _instance.reset();
  return ;
}
