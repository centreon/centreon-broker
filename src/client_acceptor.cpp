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
#include "protocol_socket.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  ClientAcceptor copy constructor.
 */
ClientAcceptor::ClientAcceptor(const ClientAcceptor& ca)
{
  (void)ca;
  assert(false);
}

/**
 *  ClientAcceptor operator= overload.
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
 *  ClientAcceptor constructor.
 */
ClientAcceptor::ClientAcceptor() : acceptor_(NULL), thread_(NULL) {}

/**
 *  ClientAcceptor destructor.
 */
ClientAcceptor::~ClientAcceptor()
{
  if (this->thread_)
    {
      this->acceptor_->Close();
      this->thread_->join();
      delete (this->thread_);
    }
  if (this->acceptor_)
    delete (this->acceptor_);
}

/**
 *  Core thread function which is in charge of accepting new clients.
 */
void ClientAcceptor::operator()()
{
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
	      // XXX : hold a list of NetworkInput
	      logging.LogInfo("New client incoming, " \
                              "launching processing thread...");
	      new NetworkInput(new ProtocolSocket(stream));
	    }
	}
    }
  catch (...)
    {
    }
  return ;
}

/**
 *  Launch the thread.
 */
void ClientAcceptor::Run(IO::Acceptor* acceptor)
{
  this->acceptor_ = acceptor;
  this->thread_ = new boost::thread(boost::ref(*this));
  return ;
}
