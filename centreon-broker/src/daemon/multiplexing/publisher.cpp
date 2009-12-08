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
#include "concurrency/lock.h"
#include "events/event.h"
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"

using namespace Multiplexing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Publisher default constructor.
 */
Publisher::Publisher() {}

/**
 *  \brief Publisher copy constructor.
 *
 *  Publisher is a singleton and therefore not copyable. This constructor does
 *  nothing else than a standard construction.
 *
 *  \param[in] publisher Unused.
 */
Publisher::Publisher(const Publisher& publisher)
{
  (void)publisher;
}

/**
 *  Publisher destructor.
 */
Publisher::~Publisher() {}

/**
 *  \brief Assignment operator overload.
 *
 *  Publisher is a singleton and therefore not copyable. This method does
 *  nothing.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] publisher Unused.
 *
 *  \return *this
 */
Publisher& Publisher::operator=(const Publisher& publisher)
{
  (void)publisher;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Get the single instance of Publisher.
 *
 *  Publisher is a singleton. Therefore only one object exists. It is stored
 *  within the Instance() method as a static object. A reference to this object
 *  is returned.
 *
 *  \return The Publisher instance.
 */
Publisher& Publisher::Instance()
{
  static Publisher publisher;

  return (publisher);
}

/**
 *  \brief Publish an event to all subscribers.
 *
 *  As soon as the method returns, the Event object is owned by the Publisher,
 *  meaning that it'll be automatically destroyed when necessary.
 *
 *  \param[in] event Event to publish.
 */
void Publisher::Publish(Events::Event* event)
{
  std::list<Subscriber*>::iterator end;
  std::list<Subscriber*>::iterator it;
  Concurrency::Lock lock(this->subscribersm_);

  end = this->subscribers_.end();
  it = this->subscribers_.begin();
  if (it != end)
    {
      do
        {
          event->AddReader(*it);
          ++it;
        } while (it != end);
      for (it = this->subscribers_.begin(); it != end; ++it)
        try
          {
            // XXX : event discrimination
            (*it)->OnEvent(event);
          }
        catch (...) {}
    }
  else
    delete (event);
  return ;
}

/**
 *  \brief Subscribe to event notifications.
 *
 *  A Subscriber which subscribe to event publication will be notified of
 *  appropriate events through its OnEvent() method.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] subscriber New subscriber.
 */
void Publisher::Subscribe(Subscriber* subscriber)
{
  Concurrency::Lock lock(this->subscribersm_);

  this->subscribers_.push_front(subscriber);
  return ;
}

/**
 *  Unsubscribe from event notifications.
 *  \par Safety Strong exception safety.
 *
 *  \param[in] subscriber Already registers subscriber.
 */
void Publisher::Unsubscribe(const Subscriber* subscriber)
{
  Concurrency::Lock lock(this->subscribersm_);

  std::remove(this->subscribers_.begin(),
              this->subscribers_.end(),
              subscriber);
  return ;
}
