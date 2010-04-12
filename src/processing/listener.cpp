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
#include <memory>                        // for auto_ptr
#include <stdlib.h>                      // for abort
#include "concurrency/lock.h"
#include "interface/ndo/destination.h"
#include "interface/ndo/source.h"
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"
#include "processing/feeder.h"
#include "processing/listener.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Listener copy constructor.
 *
 *  Listener is not copyable. Therefore any attempt to use the copy constructor
 *  will result in a call to abort().
 *
 *  \param[in] listener Unused.
 */
Listener::Listener(const Listener& listener) : Concurrency::Thread()
{
  (void)listener;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Listener is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort().
 *
 *  \param[in] listener Unused.
 *
 *  \return *this
 */
Listener& Listener::operator=(const Listener& listener)
{
  (void)listener;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Run a thread on an input stream with NDO protocol.
 */
void Listener::RunNDOIn(IO::Stream* stream)
{
  std::auto_ptr<IO::Stream> s(stream);
  std::auto_ptr<FeederOnce> feeder(new FeederOnce);
  std::auto_ptr<Multiplexing::Publisher> publishr(new Multiplexing::Publisher);
  std::auto_ptr<Interface::NDO::Source>
    source(new Interface::NDO::Source(s.get()));

  s.release();
  feeder->Run(source.get(),
              publishr.get(),
              this->listener);
  source.release();
  publishr.release();
  feeder.release();
  return ;
}

/**
 *  Run a thread on an output stream with NDO protocol.
 */
void Listener::RunNDOOut(IO::Stream* stream)
{
  std::auto_ptr<IO::Stream> s(stream);
  std::auto_ptr<FeederOnce> feeder(new FeederOnce);
  std::auto_ptr<Multiplexing::Subscriber> subscribr(
    new Multiplexing::Subscriber);
  std::auto_ptr<Interface::NDO::Destination>
    destination(new Interface::NDO::Destination(s.get()));

  s.release();
  feeder->Run(subscribr.get(),
              destination.get(),
              this->listener);
  subscribr.release();
  destination.release();
  feeder.release();
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Listener default constructor.
 */
Listener::Listener() : init_(false) {}

/**
 *  Listener destructor.
 */
Listener::~Listener()
{
  try
    {
      Concurrency::Lock lock(this->initm_);

      if (this->init_)
        {
          this->init_ = false;
          if (this->acceptor_.get())
            this->acceptor_->Close();
          this->Join();
        }
    }
  catch (...) {}
}

/**
 *  \brief Overload of the parenthesis operator.
 *
 *  This method is used as the entry point of the processing thread which
 *  listens on incoming connections.
 *  \param[in] No throw guarantee.
 *
 *  \param[in] acceptor Acceptor on which connections will be listened.
 */
void Listener::operator()()
{
  try
    {
      std::auto_ptr<IO::Stream> stream;

      // Wait for initial connection.
      stream.reset(this->acceptor_->Accept());

      while (stream.get())
        {
          // Process stream.
          (this->*(this->run_thread_))(stream.release());

          // Wait for new connection.
          stream.reset(this->acceptor_->Accept());
        }
    }
  catch (...) {}
  try
    {
      // Mutex already locked == destructor being run.
      if (this->initm_.TryLock())
        {
          this->init_ = false;
          this->initm_.Unlock();
        }
    }
  catch (...) {}
  return ;
}

/**
 *  Ask thread to exit ASAP.
 */
void Listener::Exit()
{
  // Set exit flag.
  this->Thread::Exit();

  // Close acceptor.
  if (this->acceptor_.get())
    this->acceptor_->Close();

  return ;
}

/**
 *  \brief Launch processing thread.
 *
 *  Launch the thread waiting on incoming connections. Upon successful return
 *  from this method, the acceptor will be owned by the Listener.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] acceptor Acceptor on which incoming clients will be awaited.
 *  \param[in] proto    Protocol to use on new connections.
 *  \param[in] io       Should we act on input or output objects ?
 *  \param[in] tl       Listener for all created threads.
 */
void Listener::Init(IO::Acceptor* acceptor,
                    Listener::Protocol proto,
                    Listener::INOUT io,
                    Concurrency::ThreadListener* tl)
{
  Concurrency::Lock lock(this->initm_);

  try
    {
      this->acceptor_.reset(acceptor);
      this->init_ = true;
      if (OUT == io)
        this->run_thread_ = &Processing::Listener::RunNDOOut;
      else
        this->run_thread_ = &Processing::Listener::RunNDOIn;
      this->Run(tl);
    }
  catch (...)
    {
      this->acceptor_.release();
      this->init_ = false;
      throw ;
    }
  return ;
}
