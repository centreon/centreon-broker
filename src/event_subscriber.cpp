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

#include "event_publisher.h"
#include "event_subscriber.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief EventSubscriber default constructor.
 *
 *  Subscribe to the EventPublisher.
 */
EventSubscriber::EventSubscriber()
{
  EventPublisher::GetInstance().Subscribe(this);
}

/**
 *  EventSubscriber copy constructor.
 *
 *  Does the same as the EventSubscriber default constructor.
 *
 *  \param[in] es Unused.
 */
EventSubscriber::EventSubscriber(const EventSubscriber& es)
{
  (void)es;
  EventPublisher::GetInstance().Subscribe(this);
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Does nothing.
 *
 *  \param[in] es Unused.
 *
 *  \return *this
 */
EventSubscriber& EventSubscriber::operator=(const EventSubscriber& es)
{
  (void)es;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief EventSubscriber destructor.
 *
 *  Unsubscribe from the EventPublisher.
 */
EventSubscriber::~EventSubscriber()
{
  EventPublisher::GetInstance().Unsubscribe(this);
}
