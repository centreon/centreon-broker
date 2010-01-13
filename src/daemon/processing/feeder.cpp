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
#include <stdlib.h>                  // for abort
#include "concurrency/lock.h"
#include "configuration/interface.h"
#include "events/event.h"
#include "interface/destination.h"
#include "interface/source.h"
#include "processing/feeder.h"

using namespace Processing;


/******************************************************************************
*                                                                             *
*                                                                             *
*                                  Feeder                                     *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Feeder copy constructor.
 *
 *  Feeder is not copyable. Any attempt to use the copy constructor will result
 *  in a call to abort().
 *
 *  \param[in] feeder Unused.
 */
Feeder::Feeder(const Feeder& feeder) : Concurrency::Thread()
{
  (void)feeder;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Feeder is not copyable. Any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] feeder Unused.
 *
 *  \return *this
 */
Feeder& Feeder::operator=(const Feeder& feeder)
{
  (void)feeder;
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
 *  Feeder default constructor.
 */
Feeder::Feeder() : exit_(true) {}

/**
 *  Feeder destructor.
 */
Feeder::~Feeder()
{
  if (!this->exit_)
    {
      this->Exit();
      this->Join();
    }
}

/**
 *  Warn the thread that it should quit ASAP.
 */
void Feeder::Exit()
{
  this->exit_ = true;
  return ;
}

/**
 *  Send events from the source to the destination.
 */
void Feeder::Feed(Interface::Source* source,
                  Interface::Destination* dest)
{
  Events::Event* event;

  event = NULL;
  try
    {
      // Fetch first event.
      event = source->Event();
      while (event)
        {
          // Send event.
	  dest->Event(event);
	  event = NULL;
          // Fetch next event.
	  event = source->Event();
        }
    }
  catch (...)
    {
      if (event)
        event->RemoveReader();
      throw ;
    }
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                FeederOnce                                   *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy constructor.
 *
 *  FeederOnce is not copyable ; therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fo Unused.
 */
FeederOnce::FeederOnce(const FeederOnce& fo)
  : Interface::Source(),
    Interface::Destination(),
    Processing::Feeder(),
    Interface::SourceDestination()
{
  (void)fo;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  FeederOnce is not copyable ; therefore any attempt to use the assignment
 *  operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fo Unused.
 *
 *  \return *this
 */
FeederOnce& FeederOnce::operator=(const FeederOnce& fo)
{
  (void)fo;
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
 *  Default constructor.
 */
FeederOnce::FeederOnce() {}

/**
 *  Destructor.
 */
FeederOnce::~FeederOnce()
{
  // XXX : thread synchronization
}

/**
 *  Feeder thread entry point.
 */
void FeederOnce::operator()()
{
  this->Feed(this, this);
  return ;
}

/**
 *  Close.
 */
void FeederOnce::Close()
{
  return ;
}

/**
 *  Get next available event.
 */
Events::Event* FeederOnce::Event()
{
  Concurrency::Lock lock(this->sourcem_);

  return (this->source_->Event());
}

/**
 *  Store event.
 */
void FeederOnce::Event(Events::Event* event)
{
  Concurrency::Lock lock(this->destm_);

  this->dest_->Event(event);
  return ;
}

/**
 *  Run feeder thread.
 */
void FeederOnce::Run(Interface::Source* source,
		     Interface::Destination* dest,
		     Concurrency::ThreadListener* tl)
{
  try
    {
      {
	Concurrency::Lock lock(this->destm_);

	this->dest_.reset(dest);
      }
      {
	Concurrency::Lock lock(this->sourcem_);

	this->source_.reset(source);
      }
      this->Concurrency::Thread::Run(tl);
    }
  catch (...)
    {
      this->dest_.reset();
      this->source_.reset();
      throw ;
    }
  return ;
}
