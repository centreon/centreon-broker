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

#include <QMutexLocker>
#include "events/event.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Copy data from the given object to the current instance.
 *
 *  @param[in] e Object to copy from.
 */
event::event(event const& e) : io::data(e) {}

/**
 *  Assignment operator.
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
event& event::operator=(event const& e) {
  io::data::operator=(e);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
event::event() {}

/**
 *  Destructor.
 */
event::~event() {}

/**
 *  Get the event memory.
 *
 *  @return Pointer to this event.
 */
void* event::memory() {
  return (this);
}

/**
 *  Get the event memory.
 *
 *  @return Pointer to this event.
 */
void const* event::memory() const {
  return (this);
}

/**
 *  Get the event size.
 *
 *  @return sizeof(event).
 */
unsigned int event::size() const {
  return (sizeof(event));
}
