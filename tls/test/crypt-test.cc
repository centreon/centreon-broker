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

/**
 * Create server/client self-signed certificate/key (self signed, DONT ADD
   PASSWORD)

 openssl req -x509 -newkey rsa:2048 -days 3650 -nodes -keyout client-key.pem -out client-cert.pem

 openssl req -x509 -newkey rsa:2048 -days 3650 -nodes -keyout server-key.pem -out server-cert.pem

*/
#include <assert.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <fmt/format.h>
#include <string.h>
#include <string>

static BIO* bio_err{nullptr};

int main() {
  bio_err = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);

  auto cert_keys([](SSL_CTX* ctx, const std::string& name) {
    /* load key and certificate */
    std::string certfile{fmt::format("./{}-cert.pem", name)};
    std::string keyfile{fmt::format("./{}-key.pem", name)};

    /* certificate file; contains also the public key */
    int r = SSL_CTX_use_certificate_file(ctx, certfile.c_str(), SSL_FILETYPE_PEM);
    if (r <= 0) {
      printf("Error: cannot load certificate file.\n");
      ERR_print_errors_fp(stderr);
      return -4;
    }

    /* load private key */
    r = SSL_CTX_use_PrivateKey_file(ctx, keyfile.c_str(), SSL_FILETYPE_PEM);
    if (r <= 0) {
      printf("Error: cannot load private key file.\n");
      ERR_print_errors_fp(stderr);
      return -5;
    }

    /* check if the private key is valid */
    r = SSL_CTX_check_private_key(ctx);
    if (r != 1) {
      printf("Error: checking the private key failed. \n");
      ERR_print_errors_fp(stderr);
      return -6;
    }
    return 0;
  });

  SSL_CTX* c_ctx = SSL_CTX_new(TLS_client_method());
  SSL_CTX* s_ctx = SSL_CTX_new(TLS_server_method());

  if (c_ctx == nullptr || s_ctx == nullptr) {
    ERR_print_errors(bio_err);
    goto end;
  }

  /* This is needed if we want to use the aNULL cipher */
  SSL_CTX_set_security_level(c_ctx, 0);
  SSL_CTX_set_security_level(s_ctx, 0);

  SSL_CTX_set_ecdh_auto(c_ctx, 1);
  SSL_CTX_set_ecdh_auto(s_ctx, 1);

  if (!SSL_CTX_set_cipher_list(s_ctx, "ALL") ||
      !SSL_CTX_set_cipher_list(c_ctx, "ALL")) {
    ERR_print_errors(bio_err);
    goto end;
  }

  if (cert_keys(s_ctx, "server") || cert_keys(c_ctx, "client")) {
    goto end;
  }

  {
    clock_t s_time = 0, c_time = 0;

    SSL* s_ssl = SSL_new(s_ctx);
    SSL* c_ssl = SSL_new(c_ctx);

    BIO *s_ssl_bio = nullptr, *c_ssl_bio = nullptr;
    BIO *server = nullptr, *server_io = nullptr, *client = nullptr,
        *client_io = nullptr;
    int err_in_client = 0;
    int err_in_server = 0;

    size_t bufsiz = 256; /* small buffer for testing */

    if (!BIO_new_bio_pair(&server, bufsiz, &server_io, bufsiz))
      goto err;
    if (!BIO_new_bio_pair(&client, bufsiz, &client_io, bufsiz))
      goto err;

    s_ssl_bio = BIO_new(BIO_f_ssl());
    if (!s_ssl_bio)
      goto err;

    c_ssl_bio = BIO_new(BIO_f_ssl());
    if (!c_ssl_bio)
      goto err;

    {
      SSL_set_connect_state(c_ssl);
      SSL_set_bio(c_ssl, client, client);
      (void)BIO_set_ssl(c_ssl_bio, c_ssl, BIO_NOCLOSE);

      SSL_set_accept_state(s_ssl);
      SSL_set_bio(s_ssl, server, server);
      (void)BIO_set_ssl(s_ssl_bio, s_ssl, BIO_NOCLOSE);

      std::string str{"Bonjour le monde!"};
      size_t count{str.size()};
      size_t cw_num = count, cr_num = count, sw_num = count, sr_num = count;
      constexpr bool debug{true};

      do {
        /*-
         * c_ssl_bio:          SSL filter BIO
         *
         * client:             pseudo-I/O for SSL library
         *
         * client_io:          client's SSL communication; usually to be
         *                     relayed over some I/O facility, but in this
         *                     test program, we're the server, too:
         *
         * server_io:          server's SSL communication
         *
         * server:             pseudo-I/O for SSL library
         *
         * s_ssl_bio:          SSL filter BIO
         *
         * The client and the server each employ a "BIO pair":
         * client + client_io, server + server_io.
         * BIO pairs are symmetric.  A BIO pair behaves similar
         * to a non-blocking socketpair (but both endpoints must
         * be handled by the same thread).
         * [Here we could connect client and server to the ends
         * of a single BIO pair, but then this code would be less
         * suitable as an example for BIO pairs in general.]
         *
         * Useful functions for querying the state of BIO pair endpoints:
         *
         * BIO_ctrl_pending(bio)              number of bytes we can read now
         * BIO_ctrl_get_read_request(bio)     number of bytes needed to fulfill
         *                                      other side's read attempt
         * BIO_ctrl_get_write_guarantee(bio)   number of bytes we can write now
         *
         * ..._read_request is never more than ..._write_guarantee;
         * it depends on the application which one you should use.
         */

        /*
         * We have non-blocking behaviour throughout this test program, but
         * can be sure that there is *some* progress in each iteration; so we
         * don't have to worry about ..._SHOULD_READ or ..._SHOULD_WRITE --
         * we just try everything in each iteration
         */

        {
          /* CLIENT */

          char cbuf[1024 * 8];
          int i, r;
          clock_t c_clock = clock();

          memset(cbuf, 0, sizeof(cbuf));
          memcpy(cbuf, str.data(), count);

          if (debug)
            if (SSL_in_init(c_ssl))
              printf("client waiting in SSL_connect - %s\n",
                     SSL_state_string_long(c_ssl));

          if (cw_num > 0) {
            /* Write to server. */
            if (cw_num > (long)sizeof(cbuf))
              i = sizeof(cbuf);
            else
              i = (int)cw_num;
            r = BIO_write(c_ssl_bio, cbuf, i);
            if (r < 0) {
              if (!BIO_should_retry(c_ssl_bio)) {
                fprintf(stderr, "ERROR in CLIENT\n");
                err_in_client = 1;
                goto err;
              }
              /*
               * BIO_should_retry(...) can just be ignored here. The
               * library expects us to call BIO_write with the same
               * arguments again, and that's what we will do in the
               * next iteration.
               */
            } else if (r == 0) {
              fprintf(stderr, "SSL CLIENT STARTUP FAILED\n");
              goto err;
            } else {
              if (debug)
                printf("client wrote %d\n", r);
              cw_num -= r;
            }
          }

          if (cr_num > 0) {
            /* Read from server. */

            r = BIO_read(c_ssl_bio, cbuf, sizeof(cbuf));
            if (r < 0) {
              if (!BIO_should_retry(c_ssl_bio)) {
                fprintf(stderr, "ERROR in CLIENT\n");
                err_in_client = 1;
                goto err;
              }
              /*
               * Again, "BIO_should_retry" can be ignored.
               */
            } else if (r == 0) {
              fprintf(stderr, "SSL CLIENT STARTUP FAILED\n");
              goto err;
            } else {
              if (debug)
                printf("client read %d '%s'\n", r, cbuf);
              cr_num -= r;
            }
          }

          /*
           * c_time and s_time increments will typically be very small
           * (depending on machine speed and clock tick intervals), but
           * sampling over a large number of connections should result in
           * fairly accurate figures.  We cannot guarantee a lot, however
           * -- if each connection lasts for exactly one clock tick, it
           * will be counted only for the client or only for the server or
           * even not at all.
           */
          c_time += (clock() - c_clock);
        }

        {
          /* SERVER */

          char sbuf[1024 * 8];
          int i, r;
          clock_t s_clock = clock();

          memset(sbuf, 0, sizeof(sbuf));

          if (debug)
            if (SSL_in_init(s_ssl))
              printf("server waiting in SSL_accept - %s\n",
                     SSL_state_string_long(s_ssl));

          if (sw_num > 0) {
            /* Write to client. */

            if (sw_num > (long)sizeof(sbuf))
              i = sizeof(sbuf);
            else
              i = (int)sw_num;
            r = BIO_write(s_ssl_bio, sbuf, i);
            if (r < 0) {
              if (!BIO_should_retry(s_ssl_bio)) {
                fprintf(stderr, "ERROR in SERVER\n");
                err_in_server = 1;
                goto err;
              }
              /* Ignore "BIO_should_retry". */
            } else if (r == 0) {
              fprintf(stderr, "SSL SERVER STARTUP FAILED\n");
              goto err;
            } else {
              if (debug)
                printf("server wrote %d\n", r);
              sw_num -= r;
            }
          }

          if (sr_num > 0) {
            /* Read from client. */

            r = BIO_read(s_ssl_bio, sbuf, sizeof(sbuf));
            if (r < 0) {
              if (!BIO_should_retry(s_ssl_bio)) {
                fprintf(stderr, "ERROR in SERVER\n");
                err_in_server = 1;
                goto err;
              }
              /* blah, blah */
            } else if (r == 0) {
              fprintf(stderr, "SSL SERVER STARTUP FAILED\n");
              goto err;
            } else {
              if (debug)
                printf("server read %d '%s'\n", r, sbuf);
              sr_num -= r;
            }
          }

          s_time += (clock() - s_clock);
        }

        {
          /* "I/O" BETWEEN CLIENT AND SERVER. */

          size_t r1, r2;
          BIO *io1 = server_io, *io2 = client_io;
          /*
           * we use the non-copying interface for io1 and the standard
           * BIO_write/BIO_read interface for io2
           */

          static int prev_progress = 1;
          int progress = 0;

          /* io1 to io2 */
          do {
            size_t num;
            int r;

            r1 = BIO_ctrl_pending(io1);
            r2 = BIO_ctrl_get_write_guarantee(io2);

            num = r1;
            if (r2 < num)
              num = r2;
            if (num) {
              char* dataptr;

              if (INT_MAX < num) /* yeah, right */
                num = INT_MAX;

              r = BIO_nread(io1, &dataptr, (int)num);
              assert(r > 0);
              assert(r <= (int)num);
              /*
               * possibly r < num (non-contiguous data)
               */
              num = r;
              r = BIO_write(io2, dataptr, (int)num);
              if (r != (int)num) { /* can't happen */
                fprintf(stderr,
                        "ERROR: BIO_write could not write "
                        "BIO_ctrl_get_write_guarantee() bytes");
                goto err;
              }
              progress = 1;

              if (debug)
                printf((io1 == client_io) ? "C->S relaying: %d bytes\n"
                                          : "S->C relaying: %d bytes\n",
                       (int)num);
            }
          } while (r1 && r2);

          /* io2 to io1 */
          {
            size_t num;
            int r;

            r1 = BIO_ctrl_pending(io2);
            r2 = BIO_ctrl_get_read_request(io1);
            /*
             * here we could use ..._get_write_guarantee instead of
             * ..._get_read_request, but by using the latter we test
             * restartability of the SSL implementation more thoroughly
             */
            num = r1;
            if (r2 < num)
              num = r2;
            if (num) {
              char* dataptr;

              if (INT_MAX < num)
                num = INT_MAX;

              if (num > 1)
                --num; /* test restartability even more thoroughly */

              r = BIO_nwrite0(io1, &dataptr);
              assert(r > 0);
              if (r < (int)num)
                num = r;
              r = BIO_read(io2, dataptr, (int)num);
              if (r != (int)num) { /* can't happen */
                fprintf(stderr,
                        "ERROR: BIO_read could not read "
                        "BIO_ctrl_pending() bytes");
                goto err;
              }
              progress = 1;
              r = BIO_nwrite(io1, &dataptr, (int)num);
              if (r != (int)num) { /* can't happen */
                fprintf(stderr,
                        "ERROR: BIO_nwrite() did not accept "
                        "BIO_nwrite0() bytes");
                goto err;
              }

              if (debug)
                printf((io2 == client_io) ? "C->S relaying: %d bytes\n"
                                          : "S->C relaying: %d bytes\n",
                       (int)num);
            }
          } /* no loop, BIO_ctrl_get_read_request now
             * returns 0 anyway */

          if (!progress && !prev_progress)
            if (cw_num > 0 || cr_num > 0 || sw_num > 0 || sr_num > 0) {
              fprintf(stderr, "ERROR: got stuck\n");
              fprintf(stderr, " ERROR.\n");
              goto err;
            }
          prev_progress = progress;
        }
      } while (cw_num > 0 || cr_num > 0 || sw_num > 0 || sr_num > 0);
    }
  err:
    ERR_print_errors(bio_err);

    BIO_free(server);
    BIO_free(server_io);
    BIO_free(client);
    BIO_free(client_io);
    BIO_free(s_ssl_bio);
    BIO_free(c_ssl_bio);
  }

end:
  BIO_free(bio_err);
  return 0;
}
