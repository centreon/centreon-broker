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

#include <boost/bind.hpp>
// POSIX specific
#include <cstdio>
#include <fcntl.h>
#include <sys/stat.h>
#include "logging.h"
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
#ifdef USE_TLS
   ,context_(na.io_service_, boost::asio::ssl::context::tlsv1)
#endif /* USE_TLS */
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
  try
    {
#ifdef USE_TLS
      if (!this->tls_)
	{
#endif /* USE_TLS */
#ifndef NDEBUG
	  logging.LogDebug("Launching asynchronous accept on socket...");
#endif /* !USE_TLS */
	  this->new_normal_socket_ = new boost::asio::ip::tcp::socket(
            this->io_service_);
	  this->acceptor_.async_accept(*this->new_normal_socket_,
                                       boost::bind(
                                         &NetworkAcceptor::HandleAccept,
                                         this,
                                         boost::asio::placeholders::error));
#ifdef USE_TLS
	}
      else
	{
# ifndef NDEBUG
	  logging.LogDebug("Launching asynchronous accept on TLS socket...");
# endif /* !NDEBUG */
	  this->new_tls_socket_ = new boost::asio::ssl::stream<
            boost::asio::ip::tcp::socket>(this->io_service_, this->context_);
	  this->acceptor_.async_accept(this->new_tls_socket_->lowest_layer(),
                                       boost::bind(
                                         &NetworkAcceptor::HandleAccept,
					 this,
					 boost::asio::placeholders::error));
	}
#endif /* USE_TLS */
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
  : acceptor_(io_service),
    io_service_(io_service)
#ifdef USE_TLS
   ,context_(io_service, boost::asio::ssl::context::tlsv1)
#endif /* USE_TLS */
{
  this->new_normal_socket_ = NULL;
# ifdef USE_TLS
  this->new_tls_socket_ = NULL;
  this->tls_ = false;
# endif /* USE_TLS */
}

/**
 *  NetworkAcceptor destructor.
 */
NetworkAcceptor::~NetworkAcceptor() throw ()
{
  if (this->acceptor_.is_open())
    this->acceptor_.close();
  if (this->new_normal_socket_)
    delete (this->new_normal_socket_);
#ifdef USE_TLS
  if (this->new_tls_socket_)
    delete (this->new_tls_socket_);
#endif /* !USE_TLS */
}

/**
 *  Accept incoming clients connections on the specified port.
 */
void NetworkAcceptor::Accept(unsigned short port) throw (Exception)
{
  try
    {
#ifndef NDEBUG
      logging.LogDebug("Listening on socket...");
#endif /* !NDEBUG */
      this->acceptor_.open(boost::asio::ip::tcp::v4());
      this->acceptor_.bind(boost::asio::ip::tcp::endpoint(
                             boost::asio::ip::tcp::v4(),
                             port));
      this->acceptor_.listen();
      this->StartAccept();
    }
  catch (boost::system::system_error& se)
    {
#ifndef NDEBUG
      logging.LogDebug("Could not listen on specified port...");
#endif /* !NDEBUG */
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
    {
#ifndef NDEBUG
      logging.LogDebug("Error occured while accept()ing connection...");
#endif /* !NDEBUG */
#ifdef USE_TLS
      if (this->tls_)
	{
	  delete (this->new_tls_socket_);
	  this->new_tls_socket_ = NULL;
	}
      else
	{
#endif /* USE_TLS */
	  delete (this->new_normal_socket_);
	  this->new_normal_socket_ = NULL;
#ifdef USE_TLS
	}
#endif /* USE_TLS */
    }
  else
    {
      NetworkInput* ni;

      try
        {
          ni = NULL;
	  logging.LogInfo("New client incoming...");
#ifdef USE_TLS
	  if (!this->tls_)
#endif /* USE_TLS */
	    ni = new NetworkInput(this->new_normal_socket_);
#ifdef USE_TLS
	  else
	    ni = new NetworkInput(this->new_tls_socket_);
#endif /* USE_TLS */
        }
      catch (std::exception& e)
        {
          // No more memory on the system so discard the new connection.
          if (ni)
            delete (ni);
          if (this->new_normal_socket_)
	    delete (this->new_normal_socket_);
#ifdef USE_TLS
	  if (this->new_tls_socket_)
	    delete (this->new_tls_socket_);
#endif /* USE_TLS */
        }
    }
  this->new_normal_socket_ = NULL;
#ifdef USE_TLS
  this->new_tls_socket_ = NULL;
#endif /* USE_TLS */
  this->StartAccept();
  return ;
}

#ifdef USE_TLS
/**
 *  Activate TLS on acceptor.
 */
void NetworkAcceptor::SetTls(const std::string& certificate,
			     const std::string& key,
			     const std::string& ca)
{
  if (!certificate.empty() && !key.empty())
    {
# ifndef NDEBUG
      logging.LogDebug("Activating TLS on socket...");
# endif /* !NDEBUG */
      this->cert_ = certificate;
      this->key_ = key;
      this->ca_ = ca;
      this->context_.set_options(
        boost::asio::ssl::context::default_workarounds);
      this->context_.use_certificate_chain_file(this->cert_);
      this->context_.use_private_key_file(this->key_,
                                          boost::asio::ssl::context::pem);
      // Write the DH parameters in a temporary file
      {
	char tmp_name[L_tmpnam];
	int tmp_fd;

	if (tmpnam(tmp_name))
	  {
	    tmp_fd = open(tmp_name, O_WRONLY, S_IRUSR | S_IWUSR);
	    if (tmp_fd >= 0)
	      {
		const char* str;

		// Generated using openssl
		str = "-----BEGIN DH PARAMETERS-----\n" \
                  "MEYCQQDIdjgC7Qqiv9d2N9H9W6Q0AHpzD3yiPCv9Bvaem1HHwVR+p6Iw+8rnAO1+\n" \
		  "OwCNhPOkhAqfwAjI6cKGNeIs/X+zAgEC\n" \
		  "-----END DH PARAMETERS-----\n";
		write(tmp_fd, str, strlen(str));
		fsync(tmp_fd);
		this->context_.use_tmp_dh_file(tmp_name);
		close(tmp_fd);
		unlink(tmp_name);
	      }
	  }
      }
      if (!this->ca_.empty())
	{
	  this->context_.load_verify_file(this->ca_);
	  this->context_.set_verify_mode(
            boost::asio::ssl::context::verify_peer);
	}
      else
	this->context_.set_verify_mode(boost::asio::ssl::context::verify_none);
      this->tls_ = true;
    }
  else
    {
# ifndef NDEBUG
      logging.LogDebug("Deactivating TLS on socket...");
# endif /* !USE_TLS */
      this->ca_.clear();
      this->cert_.clear();
      this->key_.clear();
      this->tls_ = false;
    }
  return ;
}
#endif /* USE_TLS */
