/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#include <assert.h>
#include <stdlib.h>
#include "exceptions/retval.hh"
#include "io/tls/connector.hh"

using namespace io::tls;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  connector is not copyable. Therefore any attempt to use this copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] c Unused.
 */
connector::connector(connector const& c)
  : params(params::CLIENT), stream(NULL, NULL) {
  (void)c;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  connector is not copyable. Therefore any attempt to use this
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] c Unused.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  (void)c;
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
 *  Default constructor
 *
 *  @param[in] s Already connected underlying layer object.
 */
connector::connector(io::stream* s)
  : params(params::CLIENT), stream(s, NULL) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  @brief Connect to the remote TLS peer.
 *
 *  This method must be called before calling receive and send.
 */
void connector::connect() {
  gnutls_session_t* tls_session(new (gnutls_session_t));
  try {
    int ret;

    if ((ret = gnutls_init(tls_session, GNUTLS_CLIENT)) != GNUTLS_E_SUCCESS)
      throw (exceptions::retval(ret) << "TLS connection error: "
                                     << gnutls_strerror(ret));

    // Apply TLS parameters to the current session.
    apply(*tls_session);

    // Check certificate if necessary.
    check_cert(*tls_session);

    // Here we go !
    session = tls_session;
  }
  catch (...) {
    gnutls_deinit(*tls_session);
    delete (tls_session);
    throw ;
  }

  return ;
}
