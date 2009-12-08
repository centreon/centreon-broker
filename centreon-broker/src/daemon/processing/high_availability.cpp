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

#include <assert.h>
#include <stdlib.h>                       // for abort
#include "concurrency/lock.h"
#include "events/event.h"
#include "interface/sourcedestination.h"
#include "processing/delivery.h"
#include "processing/high_availability.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief HighAvailability copy constructor.
 *
 *  HighAvailability is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] ha Unused.
 */
HighAvailability::HighAvailability(const HighAvailability& ha)
  : Concurrency::ThreadListener(),
    Interface::Source(),
    Multiplexing::Subscriber()
{
  (void)ha;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  HighAvailability is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] ha Unused.
 *
 *  \return *this
 */
HighAvailability& HighAvailability::operator=(const HighAvailability& ha)
{
  (void)ha;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HighAvailability default constructor.
 */
HighAvailability::HighAvailability() {}

/**
 *  HighAvailability destructor.
 */
HighAvailability::~HighAvailability()
{
  // XXX
}

/**
 *  Close the event source (ie. deregister from Publisher).
 */
void HighAvailability::Close()
{
  // XXX
}

/**
 *  Get the next available event.
 *
 *  \return Next available event.
 */
Events::Event* HighAvailability::Event()
{
  Events::Event* event;
  Concurrency::Lock lock(this->eventsm_);

  if (!this->events_.empty())
    {
      event = this->events_.front();
      this->events_.pop_front();
    }
  else
    {
      this->eventscv_.Sleep(this->eventsm_);
      lock.Release();
      event = this->Event();
    }
  return (event);
}

/**
 *  Initialize the HighAvailability thread.
 */
void HighAvailability::Init(Interface::SourceDestination* sd)
{
  this->delivery_ = new Delivery;
  this->delivery_->Init(this, sd);
  return ;
}

/**
 *  Callback method called when a new event has been published.
 *
 *  \param[in] event New event.
 */
void HighAvailability::OnEvent(Events::Event* event)
{
  try
    {
      Concurrency::Lock lock(this->eventsm_);

      this->events_.push_back(event);
      this->eventscv_.WakeAll();
    }
  catch (...)
    {
      event->RemoveReader(this);
    }
  return ;
}
