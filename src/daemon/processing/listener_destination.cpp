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
#include "interface/xml/destination.h"
#include "multiplexing/publisher.h"
#include "multiplexing/subscriber.h"
#include "processing/feeder.h"
#include "processing/listener_destination.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief ListenerDestination copy constructor.
 *
 *  ListenerDestination is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  \param[in] ld Unused.
 */
ListenerDestination::ListenerDestination(const ListenerDestination& ld)
  : Concurrency::Thread()
{
  (void)ld;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  ListenerDestination is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  \param[in] ld Unused.
 *
 *  \return *this
 */
ListenerDestination& ListenerDestination::operator=(
  const ListenerDestination& ld)
{
  (void)ld;
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
 *  ListenerDestination default constructor.
 */
ListenerDestination::ListenerDestination() : init_(false) {}

/**
 *  ListenerDestination destructor.
 */
ListenerDestination::~ListenerDestination()
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
 */
void ListenerDestination::operator()()
{
  try
    {
      std::auto_ptr<IO::Stream> stream;

      // Wait for initial connection.
      stream.reset(this->acceptor_->Accept());

      while (stream.get())
        {
          std::auto_ptr<Interface::Destination> dest;

          // Open protocol object.
          // XXX if (XML == this->protocol_)
            dest.reset(new Interface::XML::Destination(stream.get()));
          /*else
            dest.reset(new Interface::NDO::Destination(stream.get()));*/
          stream.release();

          // Create high availability object.
          std::auto_ptr<Feeder> feeder(new Feeder);
          std::auto_ptr<Multiplexing::Subscriber> subscr(
            new Multiplexing::Subscriber);

          // XXX
          feeder->Run(*subscr,
                      *dest);
          subscr.release();
          dest.release();
          feeder.release();

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
 *  \brief Launch processing thread.
 *
 *  Launch the thread waiting on incoming connections. Upon successful return
 *  from this method, the acceptor will be owned by the ListenerDestination.
 *  \par Safety Minimal exception safety.
 *
 *  \param[in] acceptor Acceptor on which incoming clients will be awaited.
 *  \param[in] proto    Protocol to use on new connections.
 */
void ListenerDestination::Init(IO::Acceptor* acceptor,
                               ListenerDestination::Protocol proto,
                               Concurrency::ThreadListener* tl)
{
  Concurrency::Lock lock(this->initm_);

  try
    {
      this->acceptor_.reset(acceptor);
      this->init_ = true;
      this->protocol_ = proto;
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
