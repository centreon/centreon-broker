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
#include <stdlib.h>        // for abort
#include "exception.h"
#include "io/stream.h"
#include "io/tls/stream.h"

using namespace IO::TLS;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  \brief Stream copy constructor.
 *
 *  Stream is not copiable, that's why the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] stream Unused.
 */
Stream::Stream(const Stream& stream) : IO::Stream(stream)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignement operator overload.
 *
 *  Stream is not copiable, that's why the assignment operator is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] stream Unused.
 *
 *  \return *this
 */
Stream& Stream::operator=(const Stream& stream)
{
  (void)stream;
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
 *  \brief Stream constructor.
 *
 *  When building the Stream, you need to provide a stream that will be used to
 *  transport encrypted data and a TLS session, providing informations on the
 *  kind of encryption to use. Upon completion of this constructor, the Stream
 *  object is considered to be the owner of the given objects, which means that
 *  the Stream object is responsible for their destruction.
 *
 *  \param[in] lower The stream object that will transport encrypted data.
 *  \param[in] sess  TLS session, providing informations on the encryption that
 *                   should be used.
 */
Stream::Stream(IO::Stream* lower, gnutls_session_t* sess)
  : lower_(lower), session(sess){}

/**
 *  \brief Stream destructor.
 *
 *  The destructor will release all acquired ressources that haven't been
 *  released yet.
 */
Stream::~Stream()
{
  this->Close();
}

/**
 *  \brief Close the TLS stream.
 *
 *  This method will shutdown the TLS session and close the underlying stream,
 *  releasing all acquired ressources.
 */
void Stream::Close()
{
  if (this->session)
    {
      gnutls_bye(*this->session, GNUTLS_SHUT_RDWR);
      gnutls_deinit(*this->session);
      delete (this->session);
      this->session = NULL;
    }
  if (this->lower_.get())
    {
      this->lower_->Close();
      this->lower_.reset();
    }
  return ;
}

/**
 *  \brief Receive data from the TLS session.
 *
 *  Receive at most size bytes from the network stream and store them in
 *  buffer. The number of bytes read is then returned. This number can be less
 *  than size.
 *
 *  \param[out] buffer Buffer on which to store received data.
 *  \param[in]  size   Maximum number of bytes to read.
 *
 *  \return Number of bytes read from the network stream. 0 if the session has
 *          been shut down.
 */
unsigned int Stream::Receive(void* buffer, unsigned int size)
{
  int ret;

  ret = gnutls_record_recv(*this->session, buffer, size);
  if (ret < 0)
    throw (Exception(ret, gnutls_strerror(ret)));
  return ((unsigned int)ret);
}

/**
 *  \brief Send data across the TLS session.
 *
 *  Send at most size bytes from the buffer. The number of bytes actually sent
 *  is returned. This number can be less than size.
 *
 *  \param[in] buffer Data to send.
 *  \param[in] size   Maximum number of bytes to send.
 *
 *  \return Number of bytes actually sent through the TLS session. 0 if the
 *          connection has been shut down.
 */
unsigned int Stream::Send(const void* buffer, unsigned int size)
{
  int ret;

  ret = gnutls_record_send(*this->session, buffer, size);
  if (ret < 0)
    throw (Exception(ret, gnutls_strerror(ret)));
  return ((unsigned int)ret);
}
