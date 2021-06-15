/*
    Create server/client self-signed certificate/key (self signed, DONT ADD
   PASSWORD)

    openssl req -x509 -newkey rsa:2048 -days 3650 -nodes -keyout client-key.pem
   -out client-cert.pem

   openssl req -x509 -newkey rsa:2048 -days 3650 -nodes
   -keyout server-key.pem -out server-cert.pem

*/

#include <stdio.h>
#include <stdlib.h>

#include <openssl/conf.h>
#include <openssl/dh.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

/* SSL debug */
#define SSL_WHERE_INFO(ssl, w, flag, msg)                \
  {                                                      \
    if (w & flag) {                                      \
      printf("+ %s: ", name);                            \
      printf("%20.20s", msg);                            \
      printf(" - %30.30s ", SSL_state_string_long(ssl)); \
      printf(" - %5.10s ", SSL_state_string(ssl));       \
      printf("\n");                                      \
    }                                                    \
  }

typedef void (*info_callback)(const SSL*, int, int);

typedef struct {
  SSL_CTX* ctx; /* main ssl context */
  SSL* ssl;     /* the SSL* which represents a "connection" */
  BIO* in_bio;  /* we use memory read bios */
  BIO* out_bio; /* we use memory write bios */
  char name[512];
} krx;

int krx_ssl_shutdown(krx* k); /* cleanup SSL allocated mem */
int krx_ssl_verify_peer(
    int ok,
    X509_STORE_CTX*
        ctx); /* we set the SSL_VERIFY_PEER option on the SSL_CTX, so that the
                 server will request the client certificate. We can use the
                 certificate to get/verify the fingerprint */
int krx_ssl_handle_traffic(krx* from, krx* to);

/* some debug info */
void krx_ssl_server_info_callback(
    const SSL* ssl,
    int where,
    int ret); /* purely for debug purposes; logs server info. */
void krx_ssl_client_info_callback(const SSL* ssl,
                                  int where,
                                  int ret); /* client info callback */
void krx_ssl_info_callback(const SSL* ssl,
                           int where,
                           int ret,
                           const char* name); /* generic info callback */

/**
 * @brief init the SSL* (the "connection"). we use the `isserver` to tell
 *        SSL that it should either use the server or client protocol
 *
 * @param k the internal structure for a server/client
 * @param keyname The name ("server" or "client")
 *
 * @return 0 on success.
 */
static int krx_ssl_ctx_init(krx* k, const char* keyname) {
  int r = 0;

  /* create a new context using TLS */
  k->ctx = SSL_CTX_new(TLS_method());
  if (!k->ctx) {
    printf("Error: cannot create SSL_CTX.\n");
    ERR_print_errors_fp(stderr);
    return -1;
  }

  SSL_CTX_set_ecdh_auto(ctx, 1);
//  /* set our supported ciphers */
//  r = SSL_CTX_set_cipher_list(k->ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
//  //r = SSL_CTX_set_cipher_list(k->ctx, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
//  //r = SSL_CTX_set_cipher_list(k->ctx, "aNULL");
//  if (r != 1) {
//    printf("Error: cannot set the cipher list.\n");
//    ERR_print_errors_fp(stderr);
//    return -2;
//  }
//
//  /* the client doesn't have to send it's certificate */
//  SSL_CTX_set_verify(k->ctx, SSL_VERIFY_PEER, krx_ssl_verify_peer);
//
//  /* enable srtp */
//  r = SSL_CTX_set_tlsext_use_srtp(k->ctx, "SRTP_AES128_CM_SHA1_80");
//  if (r != 0) {
//    printf("Error: cannot setup srtp.\n");
//    ERR_print_errors_fp(stderr);
//    return -3;
//  }

  /* load key and certificate */
  char certfile[1024];
  char keyfile[1024];
  sprintf(certfile, "./%s-cert.pem", keyname);
  sprintf(keyfile, "./%s-key.pem", keyname);

  /* certificate file; contains also the public key */
  r = SSL_CTX_use_certificate_file(k->ctx, certfile, SSL_FILETYPE_PEM);
  if (r <= 0) {
    printf("Error: cannot load certificate file.\n");
    ERR_print_errors_fp(stderr);
    return -4;
  }

  /* load private key */
  r = SSL_CTX_use_PrivateKey_file(k->ctx, keyfile, SSL_FILETYPE_PEM);
  if (r <= 0) {
    printf("Error: cannot load private key file.\n");
    ERR_print_errors_fp(stderr);
    return -5;
  }

  /* check if the private key is valid */
  r = SSL_CTX_check_private_key(k->ctx);
  if (r != 1) {
    printf("Error: checking the private key failed. \n");
    ERR_print_errors_fp(stderr);
    return -6;
  }

  sprintf(k->name, "+ %s", keyname);

  return 0;
}

int krx_ssl_verify_peer(int ok, X509_STORE_CTX* ctx) {
  return 1;
}

/**
 * @brief Sets up the SSL object.
 *
 * @param k The internal structure
 * @param isserver A boolean telling if k represents a server or not
 * @param cb The callback info.
 *
 * @return 0 on success.
 */
int krx_ssl_init(krx* k, bool isserver, info_callback cb) {
  /* create SSL* */
  k->ssl = SSL_new(k->ctx);
  if (!k->ssl) {
    printf("Error: cannot create new SSL*.\n");
    return -1;
  }

  /* info callback */
  SSL_set_info_callback(k->ssl, cb);

  /* bios */
  k->in_bio = BIO_new(BIO_s_mem());
  if (k->in_bio == NULL) {
    printf("Error: cannot allocate read bio.\n");
    return -2;
  }

  BIO_set_mem_eof_return(
      k->in_bio,
      -1); /* see: https://www.openssl.org/docs/crypto/BIO_s_mem.html */

  k->out_bio = BIO_new(BIO_s_mem());
  if (k->out_bio == NULL) {
    printf("Error: cannot allocate write bio.\n");
    return -3;
  }

  BIO_set_mem_eof_return(
      k->out_bio,
      -1); /* see: https://www.openssl.org/docs/crypto/BIO_s_mem.html */

  SSL_set_bio(k->ssl, k->in_bio, k->out_bio);

  /* either use the server or client part of the protocol */
  if (isserver) {
    SSL_set_accept_state(k->ssl);
  } else {
    SSL_set_connect_state(k->ssl);
  }

  return 0;
}

static long send_to_other(BIO* b, int oper, const char* argp, int argi, long argl, long retval) {
  if (argp == nullptr)
    return retval;

  if (oper == BIO_CB_WRITE | BIO_CB_RETURN) {
    int pending = BIO_ctrl_pending(b);
    krx* to = reinterpret_cast<krx*>(BIO_get_callback_arg(b));
    printf("send_to_other to %s ; pending = %d\n", to->name, pending);
    return retval;

    char outbuf[4096];
    int written = 0;
    int read = 0;
    if (pending > 0)
      read = BIO_read(b, outbuf, sizeof(outbuf));

    if (read > 0)
      written = BIO_write(to->in_bio, outbuf, read);

    if (written > 0) {
      if (!SSL_is_init_finished(to->ssl)) {
        SSL_do_handshake(to->ssl);
      } else {
        read = SSL_read(to->ssl, outbuf, sizeof(outbuf));
        printf("%s read: %s\n", to->name, outbuf);
      }
    }

  }
  return retval;
}

void prepare_callbacks(krx* server, krx* client) {
  BIO_set_callback(server->out_bio, send_to_other);
  BIO_set_callback_arg(server->out_bio, reinterpret_cast<char*>(client));
  BIO_set_callback(client->out_bio, send_to_other);
  BIO_set_callback_arg(client->out_bio, reinterpret_cast<char*>(server));
}

void krx_ssl_server_info_callback(const SSL* ssl, int where, int ret) {
  krx_ssl_info_callback(ssl, where, ret, "server");
}
void krx_ssl_client_info_callback(const SSL* ssl, int where, int ret) {
  krx_ssl_info_callback(ssl, where, ret, "client");
}

void krx_ssl_info_callback(const SSL* ssl,
                           int where,
                           int ret,
                           const char* name) {
  if (ret == 0) {
    printf("-- krx_ssl_info_callback: error occured.\n");
    return;
  }

  SSL_WHERE_INFO(ssl, where, SSL_CB_LOOP, "LOOP");
  SSL_WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_START, "HANDSHAKE START");
  SSL_WHERE_INFO(ssl, where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
}

int krx_ssl_handle_traffic(krx* from, krx* to) {
  // Did SSL write something into the out buffer
  char outbuf[4096];
  int written = 0;
  int read = 0;
  int pending = BIO_ctrl_pending(from->out_bio);

  if (pending > 0) {
    read = BIO_read(from->out_bio, outbuf, sizeof(outbuf));
  }
  printf("%s Pending %d, and read: %d\n", from->name, pending, read);

  if (read > 0) {
    written = BIO_write(to->in_bio, outbuf, read);
  }

  if (written > 0) {
    if (!SSL_is_init_finished(to->ssl)) {
      SSL_do_handshake(to->ssl);
    } else {
      read = SSL_read(to->ssl, outbuf, sizeof(outbuf));
      printf("%s read: %s\n", to->name, outbuf);
    }
  }

  return 0;
}

int krx_ssl_shutdown(krx* k) {
  if (!k)
    return -1;

  if (k->ctx) {
    SSL_CTX_free(k->ctx);
    k->ctx = NULL;
  }

  if (k->ssl) {
    SSL_free(k->ssl);
    k->ssl = NULL;
  }

  return 0;
}

int main() {
  /* startup SSL */
  //SSL_library_init();
  SSL_load_error_strings();
  OpenSSL_add_ssl_algorithms();
  ERR_load_BIO_strings();

  /* create client/server objects */
  krx server;
  krx client;

  /* init server. */
  if (krx_ssl_ctx_init(&server, "server") < 0)
    exit(EXIT_FAILURE);

  if (krx_ssl_init(&server, true, krx_ssl_server_info_callback) < 0)
    exit(EXIT_FAILURE);

  printf("+ Initialized server.\n");

  /* init client. */
  if (krx_ssl_ctx_init(&client, "client") < 0)
    exit(EXIT_FAILURE);

  if (krx_ssl_init(&client, false, krx_ssl_client_info_callback) < 0)
    exit(EXIT_FAILURE);

  prepare_callbacks(&server, &client);

  printf("+ Initialized client.\n");

  /* kickoff handshake; initiated by client (e.g. browser) */
  SSL_do_handshake(client.ssl);
  krx_ssl_handle_traffic(&client, &server);
  krx_ssl_handle_traffic(&server, &client);
  krx_ssl_handle_traffic(&client, &server);
  krx_ssl_handle_traffic(&server, &client);

  /* encrypt some data and send it to the client */
  char buf[521] = {0};
  sprintf(buf, "%s", "Hello world");
  SSL_write(server.ssl, buf, sizeof(buf));
  krx_ssl_handle_traffic(&server, &client);

  krx_ssl_shutdown(&server);
  krx_ssl_shutdown(&client);

  ERR_free_strings();
  EVP_cleanup();
  return EXIT_SUCCESS;
}

