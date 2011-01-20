/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "concurrency/lock.hh"
#include "events/event.hh"

using namespace events;

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
event::event(event const& e) {
  (void)e;
  _readers = 0;
}

/**
 *  Assignment operator.
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
event& event::operator=(event const& e) {
  (void)e;
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
event::event() : _readers(0) {}

/**
 *  Destructor.
 */
event::~event() {}

/**
 *  @brief Add a reader to the event.
 *
 *  Specify that somebody is reading the event. It shall not be
 *  destructed until the reader specify that he's done with the event.
 *
 *  @see remove_reader
 */
void event::add_reader() {
  concurrency::lock l(_mutex);
  ++_readers;
  return ;
}

/**
 *  Remove a reader from the event.
 *
 *  @see add_reader
 */
void event::remove_reader() {
  bool destroy;
  concurrency::lock l(_mutex);
  if (--_readers <= 0)
    destroy = true;
  else
    destroy = false;
  l.release();
  if (destroy)
    delete (this);
  return ;
}
