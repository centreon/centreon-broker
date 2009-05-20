/*
** network_acceptor.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/18/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#include <boost/bind.hpp>
#include "network_acceptor.h"
#include "network_input.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  NetworkAcceptor copy constructor.
 */
NetworkAcceptor::NetworkAcceptor(const NetworkAcceptor& na) throw ()
  : acceptor_(na.io_service_), io_service_(na.io_service_)
{
  (void)na;
}

/**
 *  NetworkAcceptor operator= overload.
 */
NetworkAcceptor& NetworkAcceptor::operator=(const NetworkAcceptor& na) throw ()
{
  (void)na;
  return (*this);
}

/**
 *  Start accepting a client on our acceptor.
 */
void NetworkAcceptor::StartAccept() throw ()
{
  assert(!this->new_socket_);
  try
    {
      this->new_socket_ = new boost::asio::ip::tcp::socket(this->io_service_);
      this->acceptor_.async_accept(*this->new_socket_,
                                   boost::bind(&NetworkAcceptor::HandleAccept,
                                     this,
                                     boost::asio::placeholders::error));
    }
  catch (std::exception& e)
    {
      // If an exception occured, it's because of the new() call. Because we
      // can't provide a valid socket for the asynchronous accept, we're forced
      // to shutdown (not handle asynchronous accept anymore).
    }
  return ;
}

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  NetworkAcceptor default constructor.
 */
NetworkAcceptor::NetworkAcceptor(boost::asio::io_service& io_service) throw ()
  : acceptor_(io_service), io_service_(io_service)
{
  this->new_socket_ = NULL;
}

/**
 *  NetworkAcceptor destructor.
 */
NetworkAcceptor::~NetworkAcceptor() throw ()
{
  if (this->acceptor_.is_open())
    this->acceptor_.close();
  if (this->new_socket_)
    delete (this->new_socket_);
}

/**
 *  Accept incoming clients connections on the specified port.
 */
void NetworkAcceptor::Accept(unsigned short port) throw (Exception)
{
  try
    {
      this->acceptor_.open(boost::asio::ip::tcp::v4());
      this->acceptor_.bind(boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(),
                             port));
      this->acceptor_.listen();
      this->StartAccept();
    }
  catch (boost::system::system_error& se)
    {
      if (this->acceptor_.is_open())
        this->acceptor_.close();
      throw ;
    }
  return ;
}

/**
 *  Handle an incoming connection.
 */
void NetworkAcceptor::HandleAccept(const boost::system::error_code& ec)
  throw ()
{
  // Error occured while accepting the connection. Discard the structure.
  if (ec)
    delete (this->new_socket_);
  else
    {
      NetworkInput* ni;

      try
        {
          ni = NULL;
          ni = new NetworkInput(*this->new_socket_);
        }
      catch (std::exception& e)
        {
          // No more memory on the system so discard the new connection.
          if (ni)
            delete (ni);
          else if (this->new_socket_)
            delete (this->new_socket_);
        }
    }
  this->new_socket_ = NULL;
  this->StartAccept();
  return ;
}
