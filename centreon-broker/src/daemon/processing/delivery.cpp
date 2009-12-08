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
#include "events/event.h"
#include "interface/destination.h"
#include "processing/delivery.h"
#include "processing/high_availability.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Delivery copy constructor.
 *
 *  Delivery is not copyable. Any attempt to use the copy constructor will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] delivery Unused.
 */
Delivery::Delivery(const Delivery& delivery) : Concurrency::Thread()
{
  (void)delivery;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Delivery is not copyable. Any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] delivery Unused,
 *
 *  \return *this
 */
Delivery& Delivery::operator=(const Delivery& delivery)
{
  (void)delivery;
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
 *  Delivery default constructor.
 */
Delivery::Delivery() : dest_(NULL), ha_(NULL) {}

/**
 *  Delivery destructor.
 */
Delivery::~Delivery() {}

/**
 *  Thread entry point.
 */
void Delivery::operator()()
{
  Events::Event* event;

  event = NULL;
  try
    {
      while ((event = this->ha_->Event()))
	{
          this->dest_->Event(*event);
	  event->RemoveReader(this->ha_);
	}
    }
  catch (...)
    {
      if (event)
        event->RemoveReader(this->ha_);
      // XXX : failure for HA
    }
  return ;
}

/**
 *  Launch processing thread.
 *
 *  \param[in] ha          HighAvailability object.
 *  \param[in] destination Destination object on which events will be pushed.
 */
void Delivery::Init(HighAvailability* ha, Interface::Destination* destination)
{
  this->ha_ = ha;
  this->dest_ = destination;
  this->Run();
  return ;
}
