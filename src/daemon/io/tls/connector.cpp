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
#include <stdlib.h>           // for abort
#include "exception.h"
#include "io/tls/connector.h"

using namespace IO::TLS;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Connector copy constructor.
 *
 *  Connector is not copyable. Therefore any attempt to use this copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] connector Unused.
 */
Connector::Connector(const Connector& connector)
  : Params(Params::CLIENT), Stream(NULL, NULL)
{
  (void)connector;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Connector is not copyable. Therefore any attempt to use this assignment
 *  operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] connector Unused.
 *
 *  \return *this
 */
Connector& Connector::operator=(const Connector& connector)
{
  (void)connector;
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
 *  Connector default constructor
 *
 *  \param[in] stream Already connected underlying layer object.
 */
Connector::Connector(IO::Stream* stream)
  : Params(Params::CLIENT), Stream(stream, NULL) {}

/**
 *  Connector destructor.
 */
Connector::~Connector() {}

/**
 *  \brief Connect to the remote TLS peer.
 *
 *  This method must be called before calling Receive and Send.
 */
void Connector::Connect()
{
  gnutls_session_t* tls_session;

  tls_session = new (gnutls_session_t);
  try
    {
      int ret;

      if ((ret = gnutls_init(tls_session, GNUTLS_CLIENT)) != GNUTLS_E_SUCCESS)
        throw (Exception(ret, gnutls_strerror(ret)));

      // Apply TLS parameters to the current session.
      this->Apply(*tls_session);

      // Check certificate if necessary.
      this->CheckCert(*tls_session);

      // Here we go !
      this->session = tls_session;
    }
  catch (...)
    {
      gnutls_deinit(*tls_session);
      delete (tls_session);
      throw ;
    }

  return ;
}
