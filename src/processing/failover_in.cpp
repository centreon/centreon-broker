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
#include "interface/factory.h"
#include "logging/logging.hh"
#include "processing/failover_in.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy constructor.
 *
 *  As FailoverIn is not copyable, any attempt to use the copy constructor will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fi Unused.
 */
FailoverIn::FailoverIn(const FailoverIn& fi)
  : Interface::Source(),
    Interface::Destination(),
    Feeder(),
    Interface::SourceDestination()
{
  (void)fi;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  As FailoverIn is not copyable, any attempt to use the assignment operator
 *  will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fi Unused.
 *
 *  \return *this
 */
FailoverIn& FailoverIn::operator=(const FailoverIn& fi)
{
  (void)fi;
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
FailoverIn::FailoverIn() {}

/**
 *  Destructor.
 */
FailoverIn::~FailoverIn()
{
  try
    {
      this->Exit();
      this->Join();
    }
  catch (...) {}
}

/**
 *  FailoverIn thread entry point.
 */
void FailoverIn::operator()()
{
  while (!this->should_exit)
    {
      try
	{
	  LOGDEBUG("Connecting input ...");
	  this->Connect();
	  this->Feed(this, this);
	}
      catch (...)
	{
	  LOGDEBUG("Input event feeding failed.");
	  // XXX : configure
	  sleep(5);
	}
    }
  return ;
}

/**
 *  Close.
 */
void FailoverIn::Close()
{
  // Close source.
  if (this->source_.get())
    this->source_->Close();

  return ;
}

/**
 *  Connect source stream.
 */
void FailoverIn::Connect()
{
  Concurrency::Lock lock(this->sourcem_);

  // Close before reopening.
  this->source_.reset();
  this->source_.reset(Interface::Factory::Instance().Source(
    *this->source_conf_));
  return ;
}

/**
 *  Get next available event.
 */
Events::Event* FailoverIn::Event()
{
  Concurrency::Lock lock(this->sourcem_);

  return (this->source_->Event());
}

/**
 *  Store event.
 */
void FailoverIn::Event(Events::Event* event)
{
  Concurrency::Lock lock(this->destm_);

  this->dest_->Event(event);
  return ;
}

/**
 *  Tell the thread that processing should stop.
 */
void FailoverIn::Exit()
{
  // Set exit flag.
  this->Thread::Exit();

  // Close.
  this->Close();

  return ;
}

/**
 *  Run failover thread.
 */
void FailoverIn::Run(const Configuration::Interface& source_conf,
		     Interface::Destination* destination,
		     Concurrency::ThreadListener* tl)
{
  {
    Concurrency::Lock lock(this->sourcem_);

    this->source_conf_.reset(new Configuration::Interface(source_conf));
  }
  {
    Concurrency::Lock lock(this->destm_);

    this->dest_.reset(destination);
  }
  this->Concurrency::Thread::Run(tl);
  return ;
}
