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
#include <assert.h>
#include <memory>
#include <stdlib.h>                  // for abort
#include "concurrency/lock.h"
#include "events/event.h"
#include "multiplexing/internal.h"
#include "multiplexing/subscriber.h"

using namespace Multiplexing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Subscriber copy constructor.
 *
 *  Subscriber is not copyable. Any attempt to use the copy constructor will
 *  result in a call to abort().
 *
 *  \param[in] subscriber Unused.
 */
Subscriber::Subscriber(const Subscriber& subscriber)
  : Interface::Destination(subscriber),
    Interface::Source(subscriber)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Subscriber is not copyable. Any attempt to use the assignment operator will
 *  result in a call to abort().
 *
 *  \param[in] subscriber Unused.
 *
 *  \return *this
 */
Subscriber& Subscriber::operator=(const Subscriber& subscriber)
{
  (void)subscriber;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Release all events stored within the internal list.
 */
void Subscriber::Clean()
{
  Concurrency::Lock lock(this->mutex_);

  while (!this->events_.empty())
    {
      Events::Event* event;

      event = this->events_.front();
      this->events_.pop();
      event->RemoveReader();
    }
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Subscriber default constructor.
 */
Subscriber::Subscriber()
{
  Concurrency::Lock lock(gl_subscribersm);

  gl_subscribers.push_back(this);
}

/**
 *  Subscriber destructor.
 */
Subscriber::~Subscriber()
{
  this->Clean();
  this->Close();
}

/**
 *  Unregister from event publishing notifications.
 */
void Subscriber::Close()
{
  Concurrency::Lock lock(gl_subscribersm);

  std::remove(gl_subscribers.begin(), gl_subscribers.end(), this);
  this->cv_.WakeAll();
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
      if (-1 == deadline)
        this->cv_.Sleep(this->mutex_);
      else
        this->cv_.Sleep(this->mutex_, deadline);
      if (!this->events_.empty())
        {
          event.reset(this->events_.front());
          this->events_.pop();
        }
    }
  else
    {
      event.reset(this->events_.front());
      this->events_.pop();
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

  this->events_.push(event);
  this->cv_.Wake();
  return ;
}
