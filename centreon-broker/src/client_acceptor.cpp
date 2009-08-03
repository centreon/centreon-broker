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
 *  constructor is declared private.
 *
 *  \param[in] ca Unused.
 */
ClientAcceptor::ClientAcceptor(const ClientAcceptor& ca)
{
  (void)ca;
  assert(false);
}

/**
 *  \brief Overload of the assignement operator.
 *
 *  The need to copy a client acceptor should be avoided and as such, the
 *  operator= overload is declared private.
 *
 *  \param[in] ca Unused.
 *
 *  \return *this
 */
ClientAcceptor& ClientAcceptor::operator=(const ClientAcceptor& ca)
{
  (void)ca;
  assert(false);
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
 *  Initialize internal data.
 */
ClientAcceptor::ClientAcceptor() throw (): acceptor_(NULL), thread_(NULL) {}

/**
 *  \brief ClientAcceptor destructor.
 *
 *  Release all acquired ressources.
 */
ClientAcceptor::~ClientAcceptor()
{
  if (this->thread_)
    {
      this->acceptor_->Close();
      {
	boost::unique_lock<boost::mutex> lock(this->inputsm_);

	for (std::list<NetworkInput*>::iterator it = this->inputs_.begin();
	     it != this->inputs_.end();
	     it++)
	  delete (*it);
	this->inputs_.clear();
      }
      this->thread_->join();
      delete (this->thread_);
    }
  if (this->acceptor_)
    delete (this->acceptor_);
}

/**
 *  \brief Core code of the thread.
 *
 *  When the Run() method is called, a new thread is created. This is where it
 *  starts. Basically, all ClientAcceptor processing will be in this loop. The
 *  loop it self is really simple : 1) wait for new client 2) launch processing
 *  thread.
 */
void ClientAcceptor::operator()()
{
#ifndef NDEBUG
  logging.LogDebug("Client accepting thread started.");
#endif /* !NDEBUG */
  try
    {
      IO::Stream* stream;

      while (1)
	{
	  stream = this->acceptor_->Accept();
	  if (!stream)
	    break ;
	  else
	    {
	      boost::unique_lock<boost::mutex> lock(this->inputsm_);

	      logging.LogInfo("New client incoming, " \
                              "launching processing thread...");
	      this->inputs_.push_back(new NetworkInput(this, stream));
	    }
	}
    }
  catch (std::exception& e)
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
  logging.LogInfo("Exiting client accepting thread.");
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

  it = std::find(this->inputs_.begin(), this->inputs_.end(), ni);
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
  this->acceptor_ = acceptor;
  this->thread_ = new boost::thread(boost::ref(*this));
  return ;
}
