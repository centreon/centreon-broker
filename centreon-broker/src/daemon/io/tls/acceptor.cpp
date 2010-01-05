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
#include <stdlib.h>          // for abort
#include <gnutls/gnutls.h>
#include "exception.h"
#include "io/stream.h"
#include "io/tls/acceptor.h"
#include "io/tls/internal.h"
#include "io/tls/stream.h"

using namespace IO::TLS;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Acceptor copy constructor.
 *
 *  As IO::TLS::Acceptor is not copiable, the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] acceptor Unused.
 */
Acceptor::Acceptor(const Acceptor& acceptor)
  : IO::Acceptor(acceptor), Params(Params::SERVER)
{
  (void)acceptor;
  assert(false);
  abort();
}

/**
 *  \brief Assignement operator overload.
 *
 *  As IO::TLS::Acceptor is not copiable, the assignment operator is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  \param[in] acceptor Unused.
 *
 *  \return *this
 */
Acceptor& Acceptor::operator=(const Acceptor& acceptor)
{
  (void)acceptor;
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
 *  Acceptor default constructor.
 */
Acceptor::Acceptor() : Params(Params::SERVER) {}

/**
 *  \brief Acceptor destructor.
 *
 *  Release all acquired ressources like Diffie-Hellman parameters,
 *  credentials, ... and close the underlying acceptor if it has not already
 *  been closed.
 */
Acceptor::~Acceptor()
{
  this->Close();
}

/**
 *  \brief Try to accept a new connection.
 *
 *  Wait for an incoming client through the underlying acceptor, perform TLS
 *  checks (if configured to do so) and return a TLS encrypted stream.
 *
 *  \return A TLS-encrypted stream (namely an IO::TLS::Stream object).
 *
 *  \see IO::TLS::Stream
 */
IO::Stream* Acceptor::Accept()
{
  IO::Stream* lower;
  gnutls_session_t* session;
  Stream* stream;

  /*
  ** The process of accepting a TLS client is pretty straight-forward. Just
  ** follow the comments the have an overview of performed operations.
  */

  // First accept a client from the lower layer.
  lower = this->lower_->Accept();
  session = NULL;
  stream = NULL;
  if (lower)
    {
      try
        {
          int ret;

          // Initialize the TLS session
          session = new (gnutls_session_t);
          ret = gnutls_init(session, GNUTLS_SERVER);
          if (ret != GNUTLS_E_SUCCESS)
            throw (Exception(ret, gnutls_strerror(ret)));

          // Apply TLS parameters.
          this->Apply(*session);

          // Bind the TLS session with the stream from the lower layer.
          gnutls_transport_set_lowat(*session, 0);
          gnutls_transport_set_pull_function(*session, PullHelper);
          gnutls_transport_set_push_function(*session, PushHelper);
          gnutls_transport_set_ptr(*session, lower);

          // Perform the TLS handshake.
          do
            {
              ret = gnutls_handshake(*session);
            } while (GNUTLS_E_AGAIN == ret || GNUTLS_E_INTERRUPTED == ret);
          if (ret != GNUTLS_E_SUCCESS)
            throw (Exception(ret, gnutls_strerror(ret)));

          // Check certificate.
          if (this->CheckCert(*session))
            throw (Exception(0, "Invalid certificate used in "\
                                "TLS connection."));

          stream = new Stream(lower, session);
        }
      catch (...)
        {
          if (session)
            {
              gnutls_deinit(*session);
              delete (session);
            }
          lower->Close();
          delete (lower);
          throw ;
        }
    }
  return (stream);
}

/**
 *  \brief Close the acceptor.
 *
 *  Release all acquired ressources and close the underlying Acceptor.
 */
void Acceptor::Close()
{
  if (this->lower_.get())
    {
      this->lower_->Close();
      this->lower_.reset();
    }
  return ;
}

/**
 *  \brief Get the Acceptor ready.
 *
 *  Initialize late TLS-related objects. From now on, users can call Accept()
 *  to wait for new clients. Upon a successful return of this method, the
 *  Acceptor object is considered owning the IO::Acceptor object provided,
 *  which means that the TLSAcceptor object will handle its destruction.
 *
 *  \param[in] lower The underlying Acceptor.
 */
void Acceptor::Listen(IO::Acceptor* lower)
{
  this->lower_.reset(lower);
  return ;
}
