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

#include <cassert>
#include <cstdlib>
#include "client_acceptor.h"
#include "logging.h"
#include "network_input.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief ClientAcceptor copy constructor.
 *
 *  The need to copy a client acceptor should be avoided and as such, the copy
 *  constructor is declared private. Attempt to use it anyway will result in a
 *  call to abort().
 *
 *  \param[in] ca Unused.
 */
ClientAcceptor::ClientAcceptor(const ClientAcceptor& ca)
{
  (void)ca;
  assert(false);
  abort();
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  The need to copy a client acceptor should be avoided and as such, the
 *  operator= overload is declared private. Attempt to use it anyway will
 *  result in a call to abort().
 *
 *  \param[in] ca Unused.
 *
 *  \return *this
 */
ClientAcceptor& ClientAcceptor::operator=(const ClientAcceptor& ca)
{
  (void)ca;
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
 *  \brief ClientAcceptor default constructor.
 *
 *  Initialize internal state.
 */
ClientAcceptor::ClientAcceptor() throw () {}

/**
 *  \brief ClientAcceptor destructor.
 *
 *  Release all acquired ressources.
 */
ClientAcceptor::~ClientAcceptor()
{
  // Close the acceptor socket
  if (this->acceptor_.get())
    this->acceptor_->Close();

  // Wait for the thread to finish (it will catch an exception because of the
  // closed socket acceptor).
  if (this->thread_.get())
    {
      this->thread_->join();
      this->thread_.reset();
    }

  // Delete all associated NetworkInput
  {
    boost::unique_lock<boost::mutex> lock(this->inputsm_);

    for (std::list<NetworkInput*>::iterator it = this->inputs_.begin();
         it != this->inputs_.end();
         ++it)
      delete (*it);
    this->inputs_.clear();
  }
}

/**
 *  \brief Core code of the thread.
 *
 *  When the Run() method is called, a new thread is created. This is where it
 *  starts. Basically, all ClientAcceptor processing will be in this loop. The
 *  loop it self is really simple : 1) wait for new client 2) launch processing
 *  thread.
 */
void ClientAcceptor::operator()() throw ()
{
#ifndef NDEBUG
  logging.LogDebug("Acceptor thread started !");
#endif /* !NDEBUG */
  try
    {
      while (1)
        {
          // Accept a new client.
          std::auto_ptr<IO::Stream> stream(this->acceptor_->Accept());

          // Launch the processing object and append it to the list
          {
              boost::unique_lock<boost::mutex> lock(this->inputsm_);

              logging.LogInfo("New client incoming, " \
                              "launching processing thread...");

              std::auto_ptr<NetworkInput> ni(new NetworkInput(this, stream.get()));

              // Once constructed, a NetworkInput will handle the destruction
              // of the stream
              stream.release();
              this->inputs_.push_back(ni.get());
              ni.release();
            }
        }
    }
  catch (const std::exception& e)
    {
      logging.LogError("Client acceptor failed because of an exception :");
      logging.LogError(e.what());
      logging.LogError("Exiting accepting thread.");
    }
  catch (...)
    {
      logging.LogError("Client acceptor failed because of an unknown " \
                       "exception, exiting accepting thread.");
    }
  logging.LogInfo("Exiting acceptor thread.");
  return ;
}

/**
 *  \brief Clean a terminated NetworkInput.
 *
 *  When a NetworkInput thread is over, it calls this method to release
 *  acquired ressources.
 *
 *  \param[in] ni Terminated NetworkInput.
 */
void ClientAcceptor::CleanupNetworkInput(NetworkInput* ni)
{
  boost::unique_lock<boost::mutex> lock(this->inputsm_);
  std::list<NetworkInput*>::iterator it;

  // Find the pointer in the list
  for (it = this->inputs_.begin(); it != this->inputs_.end(); it++)
    if (*it == ni)
      break ;

  // If found delete it
  if (it != this->inputs_.end())
    {
      delete (*it);
      this->inputs_.erase(it);
    }

  return ;
}

/**
 *  \brief Launch client accepting thread.
 *
 *  This method starts the processing thread.
 *
 *  \param[in] acceptor Acceptor from which clients should be accepted.
 */
void ClientAcceptor::Run(IO::Acceptor* acceptor)
{
  this->acceptor_.reset(acceptor);
  this->thread_.reset(new boost::thread(boost::ref(*this)));
  return ;
}
