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

#include <assert.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string.h>
#include <string>

static BIO* bio_err{nullptr};

int main() {
  bio_err = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);

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

  if (!SSL_CTX_set_cipher_list(s_ctx, "aNULL") ||
      !SSL_CTX_set_cipher_list(c_ctx, "aNULL")) {
    ERR_print_errors(bio_err);
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
            int r = BIO_write(c_ssl_bio, cbuf, i);
            printf("BIO_write => c_ssl_bio %d\n", r);
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
            printf("BIO_read(c_ssl_bio, ...) => %d\n", r);
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
            printf("BIO_write(s_ssl_bio, ...) => %d\n", r);
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
            printf("BIO_read(s_ssl_bio, ...) => %d\n", r);
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
          /*
           * we use the non-copying interface for server_io and the standard
           * BIO_write/BIO_read interface for client_io
           */

          static int prev_progress = 1;
          int progress = 0;

          /* server_io to client_io */
          do {
            size_t num;
            int r;

            r1 = BIO_ctrl_pending(server_io);
            printf("BIO_ctrl_pending(server_io, ...) => %d\n", r1);
            r2 = BIO_ctrl_get_write_guarantee(client_io);
            printf("BIO_ctrl_get_write_guarantee(client_io, ...) => %d\n", r2);

            num = r1;
            if (r2 < num)
              num = r2;
            if (num) {
              char* dataptr;

              if (INT_MAX < num) /* yeah, right */
                num = INT_MAX;

              r = BIO_nread(server_io, &dataptr, (int)num);
              printf("BIO_nread(server_io, ...) => %d\n", r);
              assert(r > 0);
              assert(r <= (int)num);
              /*
               * possibly r < num (non-contiguous data)
               */
              num = r;
              r = BIO_write(client_io, dataptr, (int)num);
              printf("BIO_write(client_io, ...) => %d\n", r);
              if (r != (int)num) { /* can't happen */
                fprintf(stderr,
                        "ERROR: BIO_write could not write "
                        "BIO_ctrl_get_write_guarantee() bytes");
                goto err;
              }
              progress = 1;

              if (debug)
                printf("S->C relaying: %d bytes\n", (int)num);
            }
          } while (r1 && r2);

          /* client_io to server_io */
          {
            size_t num;
            int r;

            r1 = BIO_ctrl_pending(client_io);
            printf("BIO_ctrl_pending(client_io, ...) => %d\n", r1);
            r2 = BIO_ctrl_get_read_request(server_io);
            printf("BIO_ctrl_get_read_request(server_io, ...) => %d\n", r2);
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

              r = BIO_nwrite0(server_io, &dataptr);
              printf("BIO_nwrite0(server_io, ...) => %d\n", r);
              assert(r > 0);
              if (r < (int)num)
                num = r;
              r = BIO_read(client_io, dataptr, (int)num);
              printf("BIO_read(client_io, ...) => %d\n", r);
              for (int i = 0; i < r; i++) {
                printf("%02x ", (unsigned char)dataptr[i]);
              }
              printf("\n");
              if (r != (int)num) { /* can't happen */
                fprintf(stderr,
                        "ERROR: BIO_read could not read "
                        "BIO_ctrl_pending() bytes");
                goto err;
              }
              progress = 1;
              r = BIO_nwrite(server_io, &dataptr, (int)num);
              printf("BIO_nwrite(server_io, ...) => %d\n", r);
              if (r != (int)num) { /* can't happen */
                fprintf(stderr,
                        "ERROR: BIO_nwrite() did not accept "
                        "BIO_nwrite0() bytes");
                goto err;
              }

              if (debug)
                printf("C->S relayingeeeeee: %d bytes\n", num);
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
