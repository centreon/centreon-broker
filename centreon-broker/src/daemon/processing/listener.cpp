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
#include <memory>                 // for auto_ptr
#include <stdlib.h>               // for abort
#include "concurrency/lock.h"
#include "interface/ndo/source.h"
#include "processing/feeder.h"
#include "processing/listener.h"
#include "processing/manager.h"

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
Listener::Listener(const Listener& listener)
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
      Concurrency::Lock lock(this->threadm_);

      if (this->init_)
        {
          if (this->acceptor_.get())
            this->acceptor_->Close();
          this->thread_.Join();
          this->init_ = false;
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
  std::auto_ptr<IO::Stream> stream;

  try
    {
      // Wait for initial connection.
      stream.reset(this->acceptor_->Accept());

      while (stream.get())
        {
          std::auto_ptr<Interface::Source> source;

          /* XXX         // Open protocol object.
          if (XML == this->protocol_)
            source.reset(new Interface::XML::Source(stream.get()));
            else*/
            source.reset(new Interface::NDO::Source(stream.get()));
          stream.release();

          // Create feeding thread.
          std::auto_ptr<Feeder> feeder(new Feeder);

          feeder->Init(source.get());
          source.release();
	  feeder.release();

          // Wait for new connection.
          stream.reset(this->acceptor_->Accept());
        }
    }
  catch (...) {}
  try
    {
      // Mutex already locked == destructor being run.
      if (this->threadm_.TryLock())
        {
          this->init_ = false;
          this->threadm_.Unlock();
          try { Manager::Instance().Delete(this); }
          catch (...) {}
        }
    }
  catch (...) {}
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
 */
void Listener::Init(IO::Acceptor* acceptor, Listener::Protocol proto)
{
  bool registered;

  this->acceptor_.reset(acceptor);
  this->protocol_ = proto;
  try
    {
      Concurrency::Lock lock(this->threadm_);

      registered = false;
      Manager::Instance().Manage(this);
      registered = true;
      this->thread_.Run(this);
      this->init_ = true;
    }
  catch (...)
    {
      this->acceptor_.release();
      if (registered)
        try { Manager::Instance().Delete(this); }
        catch (...) {}
      throw ;
    }
  return ;
}
