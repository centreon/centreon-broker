/*
 * Copyright 2021 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include <iostream>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

char buf[2048];
constexpr int len = 2048;

int read(SSL_CTX* ctx, BIO* ibio, BIO* obio) {
  SSL *ssl = SSL_new(ctx);

  SSL_set_bio(ssl, ibio, obio);

  int x = 0;
  for (;;) {
    int ll = BIO_read(ibio, buf + x, len - x);
    if (ll == 0) {
      std::cout << "It is over" << std::endl;
      std::cout << "Result: " << buf << std::endl;
      break;
    } else if (ll < 0) {
      if (!BIO_should_retry(ibio)) {
        std::cout << "There was an error." << std::endl;
        break;
      }
    }
    x += ll;
  }
  SSL_free(ssl);
  return 0;
}

int write(SSL* ssl, BIO* bio) {
  std::string str{"Bonjour le monde...\nComment ça va ?\n"};
  int x = 0;

  while (x < str.size()) {
    size_t ll = 0;
    int code = SSL_write_ex(ssl, str.data(), str.size(), &ll);
    if (code == 0) {
      std::cout << "There was an error during writing." << std::endl;
      return -1;
    }
    x += ll;
  }

  int retval = x;
  while (x > 0) {
    int ll = BIO_read(bio, buf, len);
    x -= ll;
  }
  return retval;
}

int main() {
  /* Initializing OpenSSL */
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  OpenSSL_add_ssl_algorithms();

  //SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
  SSL_CTX* ctx = SSL_CTX_new(TLS_method());
  if (!ctx) {
    std::cout << "Error during SSL context initialization." << std::endl;
    return -2;
  }

  SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

  SSL* ssl = SSL_new(ctx);
  BIO* rbio = BIO_new(BIO_s_mem());
  BIO* wbio = BIO_new(BIO_s_mem());

  SSL_set_bio(ssl, rbio, wbio);
  SSL_set_accept_state(ssl);

  int retval = write(ssl, ibio);
  if (retval < 0) {
    std::cout << "pas bien" << std::endl;
    return retval;
  }

  std::cout << retval << " bytes written" << std::endl;
  exit(0);
  retval = read(ctx, ibio, obio);
  if (retval < 0) {
    std::cout << "pas bien2" << std::endl;
    return retval;
  }
  SSL_CTX_free(ctx);
  BIO_free_all(ibio);
  BIO_free_all(obio);
  return 0;
}
