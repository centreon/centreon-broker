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

#include <algorithm>                 // for remove
#include <memory>                    // for auto_ptr
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
  : Interface::Destination(publisher) {}

/**
 *  Publisher destructor.
 */
Publisher::~Publisher()
{
  // Delete all Subscribers objects.
  this->subscribersm_.Lock();
  while (!this->subscribers_.empty())
    {
      Subscriber* subscriber;

      subscriber = this->subscribers_.front();
      this->subscribers_.pop_front();
      // We need to unlock the mutex while destroying the Subscriber because it
      // will call the Unsubscribe method.
      this->subscribersm_.Unlock();
      delete (subscriber);
      this->subscribersm_.Lock();
    }
  this->subscribersm_.Unlock();
}

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
 *  Does nothing on Publisher.
 */
void Publisher::Close()
{
  return ;
}

/**
 *  \brief Publish an event to all subscribers.
 *
 *  As soon as the method returns, the Event object is owned by the Publisher,
 *  meaning that it'll be automatically destroyed when necessary.
 *
 *  \param[in] event Event to publish.
 */
void Publisher::Event(Events::Event* event)
{
  std::list<Subscriber*>::iterator end;
  Concurrency::Lock lock(this->subscribersm_);

  // Add object to every subscriber.
  end = this->subscribers_.end();
  for (std::list<Subscriber*>::iterator it = this->subscribers_.begin();
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
 *  \brief Subscribe to event notifications.
 *
 *  Return an object that will automatically be filled with new events.
 *  \par Safety Strong exception safety.
 *
 *  \return New subscriber object.
 */
Subscriber* Publisher::Subscribe()
{
  Concurrency::Lock lock(this->subscribersm_);
  std::auto_ptr<Subscriber> subscriber(new Subscriber());

  this->subscribers_.push_front(subscriber.get());
  return (subscriber.release());
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
