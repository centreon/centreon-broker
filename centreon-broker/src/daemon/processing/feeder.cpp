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
#include "interface/factory.h"
#include "interface/source.h"
#include "multiplexing/publisher.h"
#include "processing/feeder.h"

using namespace Processing;

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
Feeder::Feeder() : dest_(NULL), source_(NULL), source_dest_(NULL) {}

/**
 *  Feeder destructor.
 */
Feeder::~Feeder()
{
  if (this->dest_ || this->source_)
    {
      this->Cancel();
      this->Join();
      if (this->source_conf_.get())
	delete (this->source_);
      if (this->dest_conf_.get())
	delete (this->dest_);
    }
}

/**
 *  \brief Overload of the parenthesis operator.
 *
 *  This method is used as the entry point of the thread that will get events
 *  from the source.
 *  \par Safety No throw guarantee.
 */
void Feeder::operator()()
{
  while (1)
    {
      Events::Event* event;

      event = NULL;
      try
	{
	  // Connection to source interface.
	  {
	    Concurrency::Lock lock(this->sourcem_);

	    if (this->source_conf_.get())
	      this->source_ = Interface::Factory::Instance().Source(
				*this->source_conf_);
	  }
	  // Connection to destination interface.
	  {
	    Concurrency::Lock lock(this->destm_);

	    // XXX : should be SourceDestination
	    if (this->dest_conf_.get())
	      this->dest_ = Interface::Factory::Instance().Destination(
			      *this->dest_conf_);
	  }

	  // XXX : check failover
	  {
	    Concurrency::Lock lock(this->destm_);

	    event = this->source_->Event();
	  }
	  while (event)
	    {
	      this->dest_->Event(event);
	      event = NULL;

	      Concurrency::Lock lock(this->destm_);

	      event = this->source_->Event();
	    }
	}
      catch (const std::exception& e)
	{
	}
      catch (...)
	{
	}
      if (event)
	event->RemoveReader();
      break ;
      // XXX : launch failover
    }
  return ;
}

/**
 *  Run the feeder thread.
 */
void Feeder::Run(const Configuration::Interface& source,
                 const Configuration::Interface& dest,
                 Concurrency::ThreadListener* listener)
{
  this->source_conf_.reset(new Configuration::Interface(source));
  this->dest_conf_.reset(new Configuration::Interface(dest));
  this->Run(*this->source_, *this->dest_, listener);
  return ;
}

/**
 *  Run the feeder thread.
 */
void Feeder::Run(const Configuration::Interface& source,
                 Interface::Destination& dest,
                 Concurrency::ThreadListener* listener)
{
  this->source_conf_.reset(new Configuration::Interface(source));
  this->Run(*this->source_, dest, listener);
  return ;
}

/**
 *  Run the feeder thread.
 */
void Feeder::Run(Interface::Source& source,
                 const Configuration::Interface& dest,
                 Concurrency::ThreadListener* listener)
{
  this->dest_conf_.reset(new Configuration::Interface(dest));
  this->Run(source, *this->dest_, listener);
  return ;
}

/**
 *  Run the feeder thread.
 */
void Feeder::Run(Interface::Source& source,
                 Interface::Destination& dest,
                 Concurrency::ThreadListener* listener)
{
  this->source_ = &source;
  this->dest_ = &dest;
  this->Concurrency::Thread::Run(listener);
  return ;
}
