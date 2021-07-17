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

//#include <gnutls/gnutls.h>

#include "com/centreon/broker/tls/internal.hh"
#include <cstring>
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tls/stream.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

SSL_CTX* tls::ctx = nullptr;

/**
 *  Deinit the TLS library.
 */
void tls::destroy() {
  SSL_CTX_free(ctx);
  //  // Unload Diffie-Hellman parameters.
  //  gnutls_dh_params_deinit(dh_params);
  //
  //  // Unload GNU TLS library
  //  gnutls_global_deinit();
}

/**
 *  @brief TLS initialization function.
 *
 *  Prepare all necessary ressources for TLS use.
 */
void tls::initialize() {
  ctx = SSL_CTX_new(TLS_method());
  if (ctx == nullptr)
    throw msg_fmt("Unable to allocate SSL context.");

  SSL_CTX_set_security_level(ctx, 0);
  SSL_CTX_set_ecdh_auto(ctx, 1);

  if (!SSL_CTX_set_cipher_list(ctx, "aNULL")) {
    log_v2::tls()->error("new acceptor cipher list error");
  }

  //  gnutls_datum_t const dhp = {const_cast<unsigned char*>(dh_params_2048),
  //                              sizeof(dh_params_2048)};
  //  int ret;
  //
  //  // Eventually initialize libgcrypt.
  //#if GNUTLS_VERSION_NUMBER < 0x030000
  //  log_v2::tls()->info("TLS: initializing libgcrypt (GNU TLS <= 2.11.0)");
  //  gcry_control(GCRYCTL_SET_THREAD_CBS, &gcry_threads_pthread);
  //#endif  // GNU TLS < 3.0.0
  //
  //  // Initialize GNU TLS library.
  //  if (gnutls_global_init() != GNUTLS_E_SUCCESS) {
  //    log_v2::tls()->error("TLS: GNU TLS library initialization failed");
  //    throw msg_fmt("TLS: GNU TLS library initialization failed");
  //  }
  //
  //  // Log GNU TLS version.
  //  {
  //    log_v2::tls()->info("TLS: compiled with GNU TLS version {}",
  //                        GNUTLS_VERSION);
  //    char const* v(gnutls_check_version(GNUTLS_VERSION));
  //    if (!v) {
  //      log_v2::tls()->error(
  //          "TLS: GNU TLS run-time version is incompatible with the
  //          compile-time " "version ({}): please update your GNU TLS library",
  //          GNUTLS_VERSION);
  //      throw msg_fmt(
  //          "TLS: GNU TLS run-time version is incompatible with the
  //          compile-time " "version ({}): please update your GNU TLS library",
  //          GNUTLS_VERSION);
  //    }
  //    log_v2::tls()->info("TLS: loading GNU TLS version {}", v);
  //    // gnutls_global_set_log_function(log_gnutls_message);
  //    // gnutls_global_set_log_level(11);
  //  }
  //
  //  // Load Diffie-Hellman parameters.
  //  ret = gnutls_dh_params_init(&dh_params);
  //  if (ret != GNUTLS_E_SUCCESS) {
  //    log_v2::tls()->error(
  //        "TLS: could not load TLS Diffie-Hellman parameters: {}",
  //        gnutls_strerror(ret));
  //    throw msg_fmt("TLS: could not load TLS Diffie-Hellman parameters: {}",
  //                  gnutls_strerror(ret));
  //  }
  //  ret = gnutls_dh_params_import_pkcs3(dh_params, &dhp, GNUTLS_X509_FMT_PEM);
  //  if (ret != GNUTLS_E_SUCCESS) {
  //    log_v2::tls()->error("TLS: could not import PKCS #3 parameters: ",
  //                         gnutls_strerror(ret));
  //    throw msg_fmt("TLS: could not import PKCS #3 parameters: {}",
  //                  gnutls_strerror(ret));
  //  }
}

// /**
//  *  The following static function is used to receive data from the lower
//  *  layer and give it to TLS for decoding.
//  */
// ssize_t tls::pull_helper(gnutls_transport_ptr_t ptr, void* data, size_t size)
// {
//   return static_cast<tls::stream*>(ptr)->read_encrypted(data, size);
// }

// /**
//  *  The following static function is used to send data from TLS to the lower
//  *  layer.
//  */
// ssize_t tls::push_helper(gnutls_transport_ptr_t ptr,
//                          void const* data,
//                          size_t size) {
//   return static_cast<tls::stream*>(ptr)->write_encrypted(data, size);
// }
