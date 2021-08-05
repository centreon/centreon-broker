/*
** Copyright 2021 Centreon
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

#include <openssl/x509v3.h>
#include "com/centreon/broker/tls2/internal.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

SSL_CTX* tls2::ctx = nullptr;

/**
 *  @brief TLS initialization function.
 *
 *  Prepare all necessary ressources for TLS use.
 */
void tls2::initialize() {
  ctx = SSL_CTX_new(TLS_method());
  if (ctx == nullptr)
    throw msg_fmt("Unable to allocate SSL context.");

  /* Set SSL_MODE_RELEASE_BUFFERS. This potentially greatly reduces memory usage
   */
  SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);

  X509_VERIFY_PARAM* params = SSL_CTX_get0_param(ctx);

  /* Improve trust chain building when cross-signed intermediate certificates
   * are present. Borrowed from the python ssl code. */
  X509_VERIFY_PARAM_set_flags(params, X509_V_FLAG_TRUSTED_FIRST);
  X509_VERIFY_PARAM_set_hostflags(params, X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS);
}

/**
 *  Deinit the TLS library.
 */
void tls2::destroy() {
  SSL_CTX_free(ctx);
}
