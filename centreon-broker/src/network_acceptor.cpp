/*
** network_acceptor.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/18/09 Matthieu Kermagoret
** Last update 05/18/09 Matthieu Kermagoret
*/

#include <cerrno>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include "network_acceptor.h"
#include "networkinput.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Main thread function.
 */
int NetworkAcceptor::Core()
{
  int peer;
  NetworkInput* ni;
  struct sockaddr_in sin;

  // XXX : error handling
  this->fd_ = socket(PF_INET, SOCK_STREAM, 0);
  if (this->fd_ < 0)
    {
      std::cerr << "Unable to create listening socket, exiting thread."
                << std::endl;
      return (1);
    }
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_family = AF_INET;
  sin.sin_port = htons(this->port_);
  if (bind(this->fd_, (const struct sockaddr*)&sin, sizeof(sin)))
    {
      std::cerr << "Unable to bind to local port " << ntohs(sin.sin_port)
                << ", exiting thread." << std::endl;
      return (1);
    }
  else
    std::clog << "Listening on port " << ntohs(sin.sin_port) << std::endl;
  while (!this->exit_thread_)
    {
      if (listen(this->fd_, 10))
	{
	  std::cerr << "Listen failed: " << strerror(errno) << std::endl;
	  return (1);
	}
      if ((peer = accept(this->fd_, NULL, NULL)) < 0)
	{
	  std::cerr << "Accept failed:" << strerror(errno) << std::endl;
	  return (1);
	}
      std::clog << "Incoming connection." << std::endl;
      fprintf(stderr, "Listen : %d\nPeer : %d\n", this->fd_, peer);
      ni = new (NetworkInput);
      ni->SetFD(peer);
      this->ni_.push_back(ni);
    }
  return (0);
}

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  NetworkAcceptor default constructor.
 */
NetworkAcceptor::NetworkAcceptor()
{
  this->exit_thread_ = false;
  this->fd_ = -1;
  this->port_ = 0;
}

/**
 *  NetworkAcceptor copy constructor.
 */
NetworkAcceptor::NetworkAcceptor(const NetworkAcceptor& na) : Thread()
{
  this->exit_thread_ = false;
  this->fd_ = -1;
  this->port_ = na.port_;
}

/**
 *  NetworkAcceptor destructor.
 */
NetworkAcceptor::~NetworkAcceptor()
{
  this->exit_thread_ = true;
  if (this->fd_ >= 0)
    {
      close(this->fd_);
      this->Join();
    }
  for (std::list<NetworkInput*>::iterator it = this->ni_.begin();
       it != this->ni_.end();
       it++)
    delete (*it);
}

/**
 *  NetworkAcceptor operator= overload.
 */
NetworkAcceptor& NetworkAcceptor::operator=(const NetworkAcceptor& na)
{
  this->port_ = na.port_;
  return (*this);
}

/**
 *  Get the port on which the NetworkAcceptor should listen.
 */
unsigned short NetworkAcceptor::GetPort() const throw ()
{
  return (this->port_);
}

/**
 *  Listen on the specified port.
 */
void NetworkAcceptor::Listen()
{
  this->Run();
  return ;
}

/**
 *  Set the port on which the NetworkAcceptor should listen.
 */
void NetworkAcceptor::SetPort(unsigned short port) throw ()
{
  this->port_ = port;
  return ;
}
