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
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "exception.h"
#include "io/net4.h"

using namespace CentreonBroker::IO;

/******************************************************************************
*                                                                             *
*                                                                             *
*                                  Net4Stream                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Net4Stream constructor.
 */
Net4Stream::Net4Stream(int sockfd) throw () : sockfd_(sockfd) {}

/**
 *  Net4Stream copy constructor.
 */
Net4Stream::Net4Stream(const Net4Stream& n4s) : Stream(n4s)
{
  assert(false);
}

/**
 *  Net4Stream operator= overload.
 */
Net4Stream& Net4Stream::operator=(const Net4Stream& n4s)
{
  (void)n4s;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Net4Stream destructor.
 */
Net4Stream::~Net4Stream()
{
  this->Close();
}

/**
 *  Close the network stream.
 */
void Net4Stream::Close()
{
  if (this->sockfd_ >= 0)
    {
      shutdown(this->sockfd_, SHUT_RDWR);
      close(this->sockfd_);
      this->sockfd_ = -1;
    }
  return ;
}

/**
 *  Receive data from the network stream.
 */
int Net4Stream::Receive(char* buffer, int size)
{
  // XXX : throw exception
  return (recv(this->sockfd_, buffer, size, 0));
}

/**
 *  Send data accros the network stream.
 */
int Net4Stream::Send(const char* buffer, int size)
{
  // XXX : throw exception
  return (send(this->sockfd_, buffer, size, 0));
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                               Net4Acceptor                                  *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Net4Acceptor copy constructor.
 */
Net4Acceptor::Net4Acceptor(const Net4Acceptor& n4a) throw () : Acceptor(n4a)
{
  assert(false);
}

/**
 *  Net4Acceptor operator= overload.
 */
Net4Acceptor& Net4Acceptor::operator=(const Net4Acceptor& n4a) throw ()
{
  (void)n4a;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Net4Acceptor default constructor.
 */
Net4Acceptor::Net4Acceptor() : sockfd_(-1) {}

/**
 *  Net4Acceptor destructor.
 */
Net4Acceptor::~Net4Acceptor()
{
  this->Close();
}

/**
 *  Wait for a new incoming client.
 */
Stream* Net4Acceptor::Accept()
{
  int fd;

  fd = accept(this->sockfd_, NULL, NULL);
  if (fd < 0)
    throw (CentreonBroker::Exception(fd, strerror(fd)));
  return (new Net4Stream(fd));
}

/**
 *  Close the acceptor.
 */
void Net4Acceptor::Close()
{
  if (this->sockfd_ >= 0)
    {
      shutdown(this->sockfd_, SHUT_RDWR);
      close(this->sockfd_);
      this->sockfd_ = -1;
    }
  return ;
}

/**
 *  Get port on which the acceptor is listening/will listen.
 */
unsigned short Net4Acceptor::GetPort() const throw ()
{
  return (this->port_);
}

/**
 *  Put the acceptor in listen mode.
 */
void Net4Acceptor::Listen()
{
  int ret;
  struct sockaddr_in sin;

  this->sockfd_ = socket(PF_INET, SOCK_STREAM, 0);
  if (this->sockfd_ < 0)
    throw (CentreonBroker::Exception(this->sockfd_, strerror(this->sockfd_)));
  memset(&sin, 0, sizeof(sin));
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(this->port_);
  if ((ret = bind(this->sockfd_, (struct sockaddr*)&sin, sizeof(sin)))
      || (ret = listen(this->sockfd_, 0)))
    {
      this->Close();
      throw (CentreonBroker::Exception(ret, strerror(ret)));
    }
  return ;
  return ;
}

/**
 *  Set the port on which the acceptor should listen.
 */
void Net4Acceptor::SetPort(unsigned short port)
{
  this->port_ = port;
  return ;
}
