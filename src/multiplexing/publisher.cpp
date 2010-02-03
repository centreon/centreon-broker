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

#include <algorithm>
#include <utility>
#include "concurrency/lock.h"
#include "events/events.h"
#include "multiplexing/internal.h"
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"

using namespace Multiplexing;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Publisher default constructor.
 */
Publisher::Publisher() {}

/**
 *  \brief Publisher copy constructor.
 *
 *  As Publisher does not hold any data value, this constructor is similar to
 *  the default constructor.
 *
 *  \param[in] publisher Unused.
 */
Publisher::Publisher(const Publisher& publisher)
  : Interface::Destination(publisher) {}

/**
 *  Publisher destructor.
 */
Publisher::~Publisher() {}

/**
 *  \brief Assignment operator overload.
 *
 *  As Publisher does not hold any data value, this assignment operator does
 *  nothing.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] publisher Unused.
 *
 *  \return *this
 */
Publisher& Publisher::operator=(const Publisher& publisher)
{
  this->Interface::Destination::operator=(publisher);
  return (*this);
}

/**
 *  \brief Prevent any event to be sent without error.
 *
 *  In theory Close() should prevent any event to be sent through the
 *  Event(Events::Event*) method without error. However for performance
 *  purposes, no check is actually performed and therefore this method
 *  does nothing.
 *  \par Safety No throw guarantee.
 */
void Publisher::Close()
{
  return ;
}

/**
 *  \brief Send an event to all subscribers.
 *
 *  As soon as the method returns, the Event object is owned by the Publisher,
 *  meaning that it'll be automatically destroyed when necessary.
 *  \par Safety Basic exception safety.
 *
 *  \param[in] event Event to publish.
 */
void Publisher::Event(Events::Event* event)
{
  std::list<Subscriber*>::iterator end;
  Concurrency::Lock lock(gl_subscribersm);

  // Send object to every subscriber.
  end = gl_subscribers.end();
  for (std::list<Subscriber*>::iterator it = gl_subscribers.begin();
       it != end;
       ++it)
    {
      event->AddReader();
      (*it)->Event(event);
    }
  // Self deregistration.
  event->RemoveReader();

  return ;
}
