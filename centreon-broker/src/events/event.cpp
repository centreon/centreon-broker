/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include "concurrency/lock.h"
#include "events/event.h"

using namespace Events;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief Event copy constructor.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] event Object to copy from.
 */
Event::Event(const Event& event)
{
  (void)event;
  this->readers_ = 0;
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the Nagios instance name of the given object to the current instance.
 *
 *  \param[in] event Object to copy from.
 *
 *  \return *this
 */
Event& Event::operator=(const Event& event)
{
  (void)event;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Event default constructor.
 */
Event::Event() : readers_(0) {}

/**
 *  Event destructor.
 */
Event::~Event() {}

/**
 *  \brief Add a reader to the event.
 *
 *  Specify that somebody is reading the Event. It shall not be destructed
 *  until the reader specify that he's done with the event.
 *
 *  \see RemoveReader
 */
void Event::AddReader()
{
  Concurrency::Lock lock(this->mutex_);

  ++this->readers_;
  return ;
}

/**
 *  Remove a reader from the event.
 *
 *  \see AddReader
 */
void Event::RemoveReader()
{
  bool destroy;
  Concurrency::Lock lock(this->mutex_);

  if (--this->readers_ <= 0)
    destroy = true;
  else
    destroy = false;
  lock.Release();
  if (destroy)
    delete (this);
  return ;
}
