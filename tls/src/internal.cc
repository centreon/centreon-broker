/*
** Copyright 2009-2013,2017 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <gnutls/gnutls.h>

#include <cstring>
#if GNUTLS_VERSION_NUMBER < 0x030000
#include <gcrypt.h>
#include <pthread.h>

#include <cerrno>
#endif  // GNU TLS < 3.0.0
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/tls/internal.hh"
#include "com/centreon/broker/tls/stream.hh"

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
    "-----BEGIN DH PARAMETERS-----\n"
    "MIIBCAKCAQEA93F3CN41kJooLbqcOdWHJPb+/zPV+mMs5Svb6PVH/XS3BK/tuuVu\n"
    "r9okkOzGr07KLPiKf+3MJSgHs9N91wPG6JcMcRys3fH1Tszh1i1317tE54o+oLPv\n"
    "jcs9P13lFlZm4gB7sjkR5If/ZtudoVwv7JS5WHIXrzew7iW+kT/QXCp+jkO1Vusc\n"
    "mQHlq4Fqt/p7zxOHVc8GBttE6/vEYipm2pdym1kBy62Z6rZLowkukngI5uzdQvB4\n"
    "Pmq5BmeRzGRClSkmRW4pUXiBac8SMAgMBl7cgAEaURR2D8Y4XltyXW51xzO1x1QM\n"
    "bOl9nneRY2Y8X3FOR1+Mzt+x44F+cWtqIwIBAg==\n"
    "-----END DH PARAMETERS-----\n";

gnutls_dh_params_t tls::dh_params;

#if GNUTLS_VERSION_NUMBER < 0x030000
GCRY_THREAD_OPTION_PTHREAD_IMPL;
#endif  // GNU TLS < 3.0.0

/**************************************
 *                                     *
 *          Static Functions           *
 *                                     *
 **************************************/

// Might be used below, when library logging is enabled.
// static void log_gnutls_message(int level, char const* message) {
//   (void)level;
//   logging::debug(logging::low)
//     << "TLS: GNU TLS debug: " << message;
//   return ;
// }

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

  return;
}

/**
 *  @brief TLS initialization function.
 *
 *  Prepare all necessary ressources for TLS use.
 */
void tls::initialize() {
  gnutls_datum_t const dhp = {const_cast<unsigned char*>(dh_params_2048),
                              sizeof(dh_params_2048)};
  int ret;

  // Eventually initialize libgcrypt.
#if GNUTLS_VERSION_NUMBER < 0x030000
  logging::info(logging::high)
      << "TLS: initializing libgcrypt (GNU TLS <= 2.11.0)";
  gcry_control(GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
#endif  // GNU TLS < 3.0.0

  // Initialize GNU TLS library.
  if (gnutls_global_init() != GNUTLS_E_SUCCESS) {
    log_v2::tls()->error(
        "TLS: GNU TLS library initialization failed");
    throw(exceptions::msg() << "TLS: GNU TLS library initialization failed");
  }

  // Log GNU TLS version.
  {
    logging::info(logging::medium)
        << "TLS: compiled with GNU TLS version " << GNUTLS_VERSION;
    log_v2::tls()->info("TLS: compiled with GNU TLS version {}",
                                   GNUTLS_VERSION);
    char const* v(gnutls_check_version(GNUTLS_VERSION));
    if (!v) {
      log_v2::tls()->error(
          "TLS: GNU TLS run-time version is incompatible with the compile-time "
          "version ({}): please update your GNU TLS library",
          GNUTLS_VERSION);
      throw(exceptions::msg()
            << "TLS: GNU TLS run-time version is "
            << "incompatible with the compile-time version (" << GNUTLS_VERSION
            << "): please update your GNU TLS library");
    }
    log_v2::tls()->info("TLS: loading GNU TLS version {}", v);
    logging::info(logging::high) << "TLS: loading GNU TLS version " << v;
    // gnutls_global_set_log_function(log_gnutls_message);
    // gnutls_global_set_log_level(11);
  }

  // Load Diffie-Hellman parameters.
  ret = gnutls_dh_params_init(&dh_params);
  if (ret != GNUTLS_E_SUCCESS) {
    log_v2::tls()->error(
        "TLS: could not load TLS Diffie-Hellman parameters: {}",
        gnutls_strerror(ret));
    throw(exceptions::msg()
          << "TLS: could not load TLS Diffie-Hellman parameters: "
          << gnutls_strerror(ret));
  }
  ret = gnutls_dh_params_import_pkcs3(dh_params, &dhp, GNUTLS_X509_FMT_PEM);
  if (ret != GNUTLS_E_SUCCESS) {
    log_v2::tls()->error(
        "TLS: could not import PKCS #3 parameters: ", gnutls_strerror(ret));
    throw(exceptions::msg() << "TLS: could not import PKCS #3 parameters: "
                            << gnutls_strerror(ret));
  }

  return;
}

/**
 *  The following static function is used to receive data from the lower
 *  layer and give it to TLS for decoding.
 */
ssize_t tls::pull_helper(gnutls_transport_ptr_t ptr, void* data, size_t size) {
  return (static_cast<tls::stream*>(ptr)->read_encrypted(data, size));
}

/**
 *  The following static function is used to send data from TLS to the lower
 *  layer.
 */
ssize_t tls::push_helper(gnutls_transport_ptr_t ptr,
                         void const* data,
                         size_t size) {
  return (static_cast<tls::stream*>(ptr)->write_encrypted(data, size));
}
