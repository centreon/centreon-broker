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
 *  Subscriber default constructor.
 */
Subscriber::Subscriber() {}

/**
 *  Subscriber copy constructor.
 *
 *  \param[in] subscriber Object to copy.
 */
Subscriber::Subscriber(const Subscriber& subscriber)
  : Interface::Destination(subscriber),
    Interface::Source(subscriber)
{
  this->InternalCopy(subscriber);
}

/**
 *  Assignment operator overload.
 *
 *  \param[in] subscriber Object to copy.
 *
 *  \return *this
 */
Subscriber& Subscriber::operator=(const Subscriber& subscriber)
{
  this->Clean();
  this->Interface::Destination::operator=(subscriber);
  this->Interface::Source::operator=(subscriber);
  this->InternalCopy(subscriber);
  return (*this);
}

/**
 *  Release all events stored within the internal list.
 */
void Subscriber::Clean()
{
  std::list<Events::Event*>::iterator end;
  Concurrency::Lock lock(this->mutex_);

  end = this->events_.end();
  for (std::list<Events::Event*>::iterator it = this->events_.begin();
       it != end;
       ++it)
    (*it)->RemoveReader();
  return ;
}

/**
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] subscr Object to copy.
 */
void Subscriber::InternalCopy(const Subscriber& subscr)
{
  std::list<Events::Event*>::const_iterator end;
  Concurrency::Lock lock1(this->mutex_);
  Concurrency::Lock lock2(subscr.mutex_);

  end = subscr.events_.begin();
  for (std::list<Events::Event*>::const_iterator it = subscr.events_.begin();
       it != end;
       ++it)
    {
      (*it)->AddReader();
      this->events_.push_back(*it);
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Subscriber destructor.
 */
Subscriber::~Subscriber()
{
  this->Clean();
  Publisher::Instance().Unsubscribe(this);
}

/**
 *  Does nothing.
 */
void Subscriber::Close()
{
  return ;
}

/**
 *  Get the next available event.
 *
 *  \return Next available event.
 */
Events::Event* Subscriber::Event()
{
  return (this->Event(-1));
}

/**
 *  Get the next available event without waiting after deadline.
 *
 *  \param[in] deadline Date that shouldn't be exceeded while waiting for a new
 *                      event.
 *
 *  \return Next available event, NULL if timeout occured.
 */
Events::Event* Subscriber::Event(time_t deadline)
{
  std::auto_ptr<Events::Event> event;
  Concurrency::Lock lock(this->mutex_);

  if (this->events_.empty())
    {
      this->cv_.Sleep(this->mutex_, deadline);
      if (!this->events_.empty())
        {
          event.reset(this->events_.front());
          this->events_.pop_front();
        }
    }
  else
    {
      event.reset(this->events_.front());
      this->events_.pop_front();
    }
  return (event.release());
}

/**
 *  Add a new event to the internal event list.
 *
 *  \param[in] event Event to add.
 */
void Subscriber::Event(Events::Event* event)
{
  Concurrency::Lock lock(this->mutex_);

  this->events_.push_back(event);
  this->cv_.Wake();
  return ;
}
