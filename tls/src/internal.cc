/*
** Copyright 2009-2013 Merethis
**
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
*/

#include <gnutls/gnutls.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/tls/internal.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Global Objects            *
*                                     *
**************************************/

/**
 *  Those 2048-bits wide Diffie-Hellman parameters were generated the
 *  30/07/2009 on Ubuntu 9.04 x86 using OpenSSL 0.9.8g with generator 2.
 */
unsigned char const tls::dh_params_2048[] =
  "-----BEGIN DH PARAMETERS-----\n" \
  "MIIBCAKCAQEA93F3CN41kJooLbqcOdWHJPb+/zPV+mMs5Svb6PVH/XS3BK/tuuVu\n" \
  "r9okkOzGr07KLPiKf+3MJSgHs9N91wPG6JcMcRys3fH1Tszh1i1317tE54o+oLPv\n" \
  "jcs9P13lFlZm4gB7sjkR5If/ZtudoVwv7JS5WHIXrzew7iW+kT/QXCp+jkO1Vusc\n" \
  "mQHlq4Fqt/p7zxOHVc8GBttE6/vEYipm2pdym1kBy62Z6rZLowkukngI5uzdQvB4\n" \
  "Pmq5BmeRzGRClSkmRW4pUXiBac8SMAgMBl7cgAEaURR2D8Y4XltyXW51xzO1x1QM\n" \
  "bOl9nneRY2Y8X3FOR1+Mzt+x44F+cWtqIwIBAg==\n" \
  "-----END DH PARAMETERS-----\n";

gnutls_dh_params_t tls::dh_params;

/**************************************
*                                     *
*          Global Functions           *
*                                     *
**************************************/

/**
 *  Deinit the TLS library.
 */
void tls::destroy() {
  // Unload Diffie-Hellman parameters.
  gnutls_dh_params_deinit(dh_params);

  // Unload GNU TLS library
  gnutls_global_deinit();

  return ;
}

/**
 *  @brief TLS initialization function.
 *
 *  Prepare all necessary ressources for TLS use.
 */
void tls::initialize() {
  gnutls_datum_t const dhp =
    { const_cast<unsigned char*>(dh_params_2048),
      sizeof(dh_params_2048) };
  int ret;

  // Initialize GNU TLS library.
  if (gnutls_global_init() != GNUTLS_E_SUCCESS)
    throw (exceptions::msg()
           << "TLS: GNU TLS library initialization failed");

  // Log GNU TLS version.
  {
    char const* v(gnutls_check_version(NULL));
    if (!v)
      v = "unknown";
    logging::info(logging::high)
      << "TLS: loading GNU TLS version " << v;
  }

  // Load Diffie-Hellman parameters.
  ret = gnutls_dh_params_init(&dh_params);
  if (ret != GNUTLS_E_SUCCESS)
    throw (exceptions::msg()
           << "TLS: could not load TLS Diffie-Hellman parameters: "
           << gnutls_strerror(ret));
  ret = gnutls_dh_params_import_pkcs3(
          dh_params,
          &dhp,
          GNUTLS_X509_FMT_PEM);
  if (ret != GNUTLS_E_SUCCESS)
    throw (exceptions::msg()
           << "TLS: could not import PKCS #3 parameters: "
           << gnutls_strerror(ret));

  return ;
}

/**
 *  The following static function is used to receive data from the lower
 *  layer and give it to TLS for decoding.
 */
ssize_t tls::pull_helper(
               gnutls_transport_ptr_t ptr,
               void* data,
               size_t size) {
  //return (static_cast<io::stream*>(ptr)->read(data, size));
  // XXX
}

/**
 *  The following static function is used to send data from TLS to the lower
 *  layer.
 */
ssize_t tls::push_helper(
               gnutls_transport_ptr_t ptr,
               void const* data,
               size_t size) {
  misc::shared_ptr<io::raw> r(new io::raw);
  r->append(static_cast<char const*>(data), size);
  static_cast<io::stream*>(ptr)->write(r.staticCast<io::data>());
  return (size);
}
