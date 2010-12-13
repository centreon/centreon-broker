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
#include <stdlib.h>                      // for abort()
#include "concurrency/lock.h"
#include "config/factory.hh"
#include "interface/destination.h"
#include "interface/sourcedestination.h"
#include "logging/logging.hh"
#include "processing/failover_out.h"

using namespace Processing;

/**************************************
*                                     *
*             Local Class             *
*                                     *
**************************************/

namespace              Processing
{
  class                FailoverOutAsIn : public FailoverOutBase
  {
    friend class       FailoverOutBase;

   private:
    Interface::Source* source_;
    Concurrency::Mutex sourcem_;
    std::auto_ptr<Interface::SourceDestination>
                       source_dest_;
    Concurrency::Mutex source_destm_;
    std::auto_ptr<config::interface>
                       source_dest_conf_;
                       FailoverOutAsIn(const FailoverOutAsIn& foai);
    FailoverOutAsIn&   operator=(const FailoverOutAsIn& foai);

   public:
                       FailoverOutAsIn();
                       ~FailoverOutAsIn();
    void               Close();
    void               Connect();
    Events::Event*     Event();
    void               Exit();
    void               Event(Events::Event* event);
    void               Run(Interface::Source* source,
                           const config::interface& dest_conf,
                           Concurrency::ThreadListener* tl = NULL);
  };
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                              FailoverOutBase                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief FailoverOutBase copy constructor.
 *
 *  FailoverOutBase is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fob Unused.
 */
FailoverOutBase::FailoverOutBase(const FailoverOutBase& fob)
  : Interface::Source(),
    Interface::Destination(),
    Feeder(),
    Interface::SourceDestination()
{
  (void)fob;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  FailoverOutBase is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fob Unused.
 *
 *  \return *this
 */
FailoverOutBase& FailoverOutBase::operator=(const FailoverOutBase& fob)
{
  (void)fob;
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
FailoverOutBase::FailoverOutBase() {}

/**
 *  Destructor.
 */
FailoverOutBase::~FailoverOutBase() {}

/**
 *  Failover thread entry point.
 */
void FailoverOutBase::operator()() {
  while (!this->should_exit) {
    try {
      logging::debug << logging::HIGH << "connecting output";
      this->Connect();
      logging::debug << logging::HIGH << "output connection successful";
      if (this->failover_.get()) {
        logging::debug << logging:: HIGH << "fetching events from the failover object";
        if (this->failover_->source_dest_.get())
          this->Feed(this->failover_->source_dest_.get(), this);
        logging::debug << logging::HIGH << "killing failover thread";
        this->failover_->Exit();
        this->failover_->Join();
        logging::debug << logging::HIGH << "destroying failover object";
        this->failover_.reset();
      }
      logging::debug << logging::HIGH << "launching feeding loop";
      this->Feed(this, this);
    }
    catch (std::exception const& e) {
      logging::error << logging::HIGH
                     << "error while processing output: " << e.what();
      if (!this->failover_.get() && this->dest_conf_->failover.get()) {
        logging::info << logging::HIGH << "launching output failover";
        this->failover_.reset(new FailoverOutAsIn);
        this->failover_->Run(this,
                             *this->dest_conf_->failover);
      }
      // XXX : configure
      sleep(5);
    }
    catch (...) {
      logging::error << logging::HIGH
                     << "output processing thread caught an unknown exception";
      logging::info << logging::INFO
                    << "output processing thread will now exit";
      break ;
    }
  }
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                                FailoverOut                                  *
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
 *  FailoverOut is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fo Unused.
 */
FailoverOut::FailoverOut(const FailoverOut& fo)
  : Interface::Source(), Interface::Destination(), FailoverOutBase()
{
  (void)fo;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  FailoverOut is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] fo Unused.
 *
 *  \return *this
 */
FailoverOut& FailoverOut::operator=(const FailoverOut& fo)
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
FailoverOut::FailoverOut() {}

/**
 *  Destructor.
 */
FailoverOut::~FailoverOut()
{
  this->Exit();
  this->Join();
}

/**
 *  Close current objects.
 */
void FailoverOut::Close()
{
  {
    Concurrency::Lock lock(this->sourcem_);

    if (this->source_.get())
      this->source_->Close();
  }
  {
    Concurrency::Lock lock(this->destm_);

    if (this->dest_.get())
      this->dest_->Close();
  }
  return ;
}

/**
 *  Connect the destination object.
 */
void FailoverOut::Connect()
{
  Concurrency::Lock lock(this->destm_);

  // Delete destination first before opening another.
  this->dest_.reset();
  this->dest_.reset(config::factory::build_destination(*this->dest_conf_));
  return ;
}

/**
 *  Get the next available event from the source object.
 *
 *  \return Next available event from the source object.
 */
Events::Event* FailoverOut::Event()
{
  Concurrency::Lock lock(this->sourcem_);

  return (this->source_->Event());
}

/**
 *  Store event in the destination object.
 *
 *  \param[in] event Event to store.
 */
void FailoverOut::Event(Events::Event* event)
{
  Concurrency::Lock lock(this->destm_);

  this->dest_->Event(event);
  return ;
}

/**
 *  Tell the processing thread to stop ASAP.
 */
void FailoverOut::Exit()
{
  {
    if (this->source_.get())
      this->source_->Close();
  }
  this->Feeder::Exit();
  return ;
}

/**
 *  Launch failover thread on output object.
 *
 *  \param[in] source    Source object.
 *  \param[in] dest_conf Configuration of the destination object.
 *  \param[in] tl        Listener of the thread.
 */
void FailoverOut::Run(Interface::Source* source,
                      const config::interface& dest_conf,
                      Concurrency::ThreadListener* tl)
{
  {
    Concurrency::Lock lock(this->destm_);

    this->dest_conf_.reset(new config::interface(dest_conf));
  }
  {
    Concurrency::Lock lock(this->sourcem_);

    this->source_.reset(source);
  }
  this->Concurrency::Thread::Run(tl);
  return ;
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                             FailoverOutAsIn                                 *
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
 *  FailoverOutAsIn it not copyable, therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] foai Unused.
 */
FailoverOutAsIn::FailoverOutAsIn(const FailoverOutAsIn& foai)
  : Interface::Source(), Interface::Destination(), FailoverOutBase()
{
  (void)foai;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  FailoverOutAsIn is not copyable, therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] foai Unused.
 *
 *  \return *this
 */
FailoverOutAsIn& FailoverOutAsIn::operator=(const FailoverOutAsIn& foai)
{
  (void)foai;
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
FailoverOutAsIn::FailoverOutAsIn() {}

/**
 *  Destructor.
 */
FailoverOutAsIn::~FailoverOutAsIn()
{
  try
    {
      this->Exit();
      this->Join();
    }
  catch (...) {}
}

/**
 *  Close FailoverOutAsIn.
 */
void FailoverOutAsIn::Close()
{
  Concurrency::Lock lock(this->source_destm_);

  this->source_dest_.reset();
  return ;
}

/**
 *  Connect output object.
 */
void FailoverOutAsIn::Connect()
{
  Concurrency::Lock lock(this->source_destm_);

  // Close before reopening.
  this->source_dest_.reset();
  this->source_dest_.reset(config::factory::build_sourcedestination(
    *this->source_dest_conf_));
  return ;
}

/**
 *  Get next available event.
 */
Events::Event* FailoverOutAsIn::Event()
{
  Concurrency::Lock lock(this->sourcem_);

  return (this->source_->Event());
}

/**
 *  Store event.
 */
void FailoverOutAsIn::Event(Events::Event* event)
{
  Concurrency::Lock lock(this->source_destm_);

  static_cast<Interface::Destination*>(this->source_dest_.get())->Event(event);
  return ;
}

/**
 *  Ask the thread to exit ASAP.
 */
void FailoverOutAsIn::Exit()
{
  this->Close();
  this->Feeder::Exit();
  return ;
}

/**
 *  Launch failover thread.
 */
void FailoverOutAsIn::Run(Interface::Source* source,
                          const config::interface& dest_conf,
                          Concurrency::ThreadListener* tl)
{
  {
    Concurrency::Lock lock(this->source_destm_);

    this->source_dest_conf_.reset(new config::interface(dest_conf));
  }
  {
    Concurrency::Lock lock(this->sourcem_);

    this->source_ = source;
  }
  this->Concurrency::Thread::Run(tl);
  return ;
}
