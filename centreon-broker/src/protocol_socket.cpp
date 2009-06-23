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

#include "protocol_socket.h"

using namespace CentreonBroker;

/******************************************************************************
*                                                                             *
*                                                                             *
*                              ProtocolSocket                                 *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  ProtocolSocket copy constructor.
 */
ProtocolSocket::ProtocolSocket(const ProtocolSocket& ps) throw ()
{
  (void)ps;
  assert(false);
}

/**
 *  ProtocolSocket operator= overload.
 */
ProtocolSocket& ProtocolSocket::operator=(const ProtocolSocket& ps) throw ()
{
  (void)ps;
  return (*this);
}

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Returns the next available line of input.
 */
template <typename SocketT>
char*    ProtocolSocket::GetLine(SocketT& socket)
{
  int old_length;

  this->length_ -= this->discard_;
  memmove(this->buffer_,
	  this->buffer_ + this->discard_,
	  this->length_ + 1);
  this->discard_ = 0;
  old_length = 0;
  while (!strchr(this->buffer_ + old_length, '\n')
	 && this->length_ < sizeof(this->buffer_) - 1)
    {
      unsigned long bytes_read;

      old_length = this->length_;
      bytes_read = socket->read_some(
        boost::asio::buffer(this->buffer_ + this->length_,
                            sizeof(this->buffer_) - this->length_ - 1));
      this->bytes_processed_ += bytes_read;
      this->length_ += bytes_read;
      this->buffer_[this->length_] = '\0';
      this->last_checkin_time_ = time(NULL);
    }
  this->discard_ = strcspn(this->buffer_, "\n");
  this->buffer_[this->discard_++] = '\0';
  ++this->lines_processed_;
  return (this->buffer_);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  ProtocolSocket constructor.
 */
ProtocolSocket::ProtocolSocket() throw ()
{
  this->buffer_[0] = '\0';
  this->bytes_processed_ = 0L;
  this->discard_ = 0;
  this->last_checkin_time_ = time(NULL);
  this->length_ = 0;
  this->lines_processed_ = 0L;
}

/**
 *  ProtocolSocket destructor.
 */
ProtocolSocket::~ProtocolSocket()
{
}

/**
 *  Returns the number of bytes processed by the socket.
 */
unsigned long ProtocolSocket::GetBytesProcessed() const
{
  return (this->bytes_processed_);
}

/**
 *  Returns the last time the client had an activity.
 */
time_t ProtocolSocket::GetLastCheckinTime() const
{
  return (this->last_checkin_time_);
}

/**
 *  Returns the number of lines processed by the socket.
 */
unsigned long ProtocolSocket::GetLinesProcessed() const
{
  return (this->lines_processed_);
}


/******************************************************************************
*                                                                             *
*                                                                             *
*                           StandardProtocolSocket                            *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  StandardProtocolSocket copy constructor.
 */
StandardProtocolSocket::StandardProtocolSocket(const StandardProtocolSocket& s)
  : ProtocolSocket()
{
  (void)s;
  assert(false);
}

/**
 *  StandardProtocolSocket operator= overload.
 */
StandardProtocolSocket& StandardProtocolSocket::operator=(
  const StandardProtocolSocket& sps)
{
  (void)sps;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  StandardProtocolSocket constructor.
 */
StandardProtocolSocket::StandardProtocolSocket(boost::asio::ip::tcp::socket* s)
  : socket_(s)
{
}

/**
 *  StandardProtocolSocket destructor.
 */
StandardProtocolSocket::~StandardProtocolSocket()
{
  this->socket_->close();
}

/**
 *  Get the next available line of input on the socket.
 */
char* StandardProtocolSocket::GetLine()
{
  return (this->ProtocolSocket::GetLine(this->socket_));
}


#ifdef USE_TLS
/******************************************************************************
*                                                                             *
*                                                                             *
*                            TlsProtocolSocket                                *
*                                                                             *
*                                                                             *
******************************************************************************/

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  TlsProtocolSocket copy constructor.
 */
TlsProtocolSocket::TlsProtocolSocket(const TlsProtocolSocket& tps)
  : ProtocolSocket()
{
  (void)tps;
  assert(false);
}

/**
 *  TlsProtocolSocket operator= overload.
 */
TlsProtocolSocket& TlsProtocolSocket::operator=(const TlsProtocolSocket& tps)
{
  (void)tps;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  TlsProtocolSocket constructor.
 */
TlsProtocolSocket::TlsProtocolSocket(
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* s)
  : socket_(s)
{
}

/**
 *  TlsProtocolSocket destructor.
 */
TlsProtocolSocket::~TlsProtocolSocket()
{
  this->socket_->lowest_layer().close();
}

/**
 *  Get the next available line of input on the socket.
 */
char* TlsProtocolSocket::GetLine()
{
  return (this->ProtocolSocket::GetLine(this->socket_));
}
#endif /* USE_TLS */
