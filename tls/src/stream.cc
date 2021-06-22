/*
** Copyright 2009-2017 Centreon
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

#include "com/centreon/broker/tls/stream.hh"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;
using namespace com::centreon::exceptions;

/**
 *  @brief Constructor.
 *
 *  When building the stream, you need to provide the session that will
 *  be used to transport encrypted data.
 *
 *  @param[in] sess  TLS session, providing informations on the
 *                   encryption that should be used.
 */
stream::stream(SSL* s_ssl, BIO* s_ssl_bio, BIO* s_ssl_bio_io)
    : io::stream("TLS"),
      _deadline((time_t)-1),
      _ssl(s_ssl),
      _ssl_bio(s_ssl_bio),
      _ssl_bio_io(s_ssl_bio_io) {}

/**
 *  @brief Destructor.
 *
 *  The destructor will release all acquired ressources that haven't
 *  been released yet.
 */
stream::~stream() {
  if (_ssl) {
    try {
      // _deadline = time(nullptr) + 30;  // XXX : use connection timeout
      // gnutls_bye(*_session, GNUTLS_SHUT_RDWR);
      // gnutls_deinit(*_session);
      // delete (_session);
      // _session = nullptr;
    }
    // Ignore exception whatever the error might be.
    catch (...) {
    }
  }
}

/**
 *  @brief Receive data from the TLS session.
 *
 *  Receive at most size bytes from the network stream and store them in
 *  buffer. The number of bytes read is then returned. This number can
 *  be less than size.
 *
 *  @param[out] d         Object that will be returned containing a
 *                        chunk of data.
 *  @param[in]  deadline  Timeout.
 *
 *  @return Respect io::stream::read()'s return value.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  log_v2::tls()->error("tls on start read");
  // size_t r1;
  // r1 = BIO_ctrl_get_write_guarantee(_ssl_bio_io);
  // BIO_write(_ssl_bio_io, &d, (int)r1);
  // Clear existing content.
  
  bool timed_out{false};

  std::shared_ptr<io::data> packet;
  log_v2::tls()->info("avant substream read");
  timed_out = !_substream->read(packet, _deadline);
  log_v2::tls()->info("timed out ? {}", timed_out);
  if (!timed_out) {
    if (SSL_in_init(_ssl)) {
      log_v2::tls()->error("client waiting in SSL_accept - {}",
                        SSL_state_string_long(_ssl));
      io::raw* ra(static_cast<io::raw*>(packet.get()));
      int r = BIO_write(_ssl_bio_io, ra->data(), ra->size());
      if (r < 0) {
        if (!BIO_should_retry(_ssl_bio)) {
          log_v2::tls()->error("ERROR in write CLIENT");
          // err_in_client = 1;
          return false;
        }
      /*
      * Again, "BIO_should_retry" can be ignored.
      */
      } else if (r == 0) {
        log_v2::tls()->error("SSL CLIENT STARTUP FAILED");
        return false;
      } else {
        log_v2::tls()->error("client write {} bytes", r);
      }

      // int rr = BIO_ctrl_pending(_ssl_bio_io);
      // int rr2 = BIO_ctrl_wpending(_ssl_bio_io);
      // log_v2::tls()->info("Data pending? {}", rr);
      // log_v2::tls()->info("Data pending w? {}", rr2);
      if (r) {
        char* dataptr = nullptr;
        size_t rr = BIO_nread(_ssl_bio_io, &dataptr, r);
        log_v2::tls()->info("rr = {}", rr);
        auto to_send{std::make_shared<io::raw>(&dataptr, rr)};
        _substream->write(to_send);

        // FIXME: surely not necessary.
        _substream->flush();
        return true;
      }
    } else {
      io::raw* ra(static_cast<io::raw*>(packet.get()));
      int r = BIO_write(_ssl_bio_io, ra, ra->size());
      if (r < 0) {
        if (!BIO_should_retry(_ssl_bio)) {
          log_v2::tls()->error("ERROR in write CLIENT");
          // err_in_client = 1;
          return false;
        }
      /*
      * Again, "BIO_should_retry" can be ignored.
      */
      } else if (r == 0) {
        log_v2::tls()->error("SSL CLIENT STARTUP FAILED");
        return false;
      } else {
        log_v2::tls()->error("client read {}", r);
      }
    
      log_v2::tls()->error("error timed out");
      
      std::unique_ptr<io::raw> buffer(new io::raw);
      log_v2::tls()->error("read size d {}", sizeof(d));
      r = BIO_read(_ssl_bio, &d, sizeof(d));
      if (r < 0) {
        if (!BIO_should_retry(_ssl_bio)) {
          log_v2::tls()->error("ERROR in CLIENT");
          // err_in_client = 1;
          return false;
        }
        /*
        * Again, "BIO_should_retry" can be ignored.
        */
      } else if (r == 0) {
        log_v2::tls()->error("SSL CLIENT STARTUP FAILED");
        return false;
      } else {
        log_v2::tls()->error("client read {}", r);
      }
    }
  } else if (timed_out)
    return true;
  // _substream->read(d);
  // _substream->flush();
  return 1;
}

/**
 *  @brief Send data across the TLS session.
 *
 *  Send a chunk of data.
 *
 *  @param[in] d Packet to send.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(const std::shared_ptr<io::data>& d) {
  assert(d);

  log_v2::tls()->error("tls on start write");
  if (SSL_in_init(_ssl))
    log_v2::tls()->error("server waiting in SSL_accept - {}",
                         SSL_state_string_long(_ssl));

  io::raw* packet{static_cast<io::raw*>(d.get())};
  log_v2::tls()->error("write size d {}", packet->size());

  int nb = BIO_write(_ssl_bio, packet->data(), packet->size());
  if (nb < 0) {
    if (!BIO_should_retry(_ssl_bio)) {
      log_v2::tls()->error("ERROR in SERVER");

      // err_in_server = 1;
      // goto err;
    }
    /* Ignore "BIO_should_retry". */
  } else if (nb == 0) {
    log_v2::tls()->error("SSL SERVER STARTUP FAILED");
  } else {
    log_v2::tls()->debug("server wrote {}", nb);
    char* dataptr;
    size_t toto = BIO_nwrite0(_ssl_bio, &dataptr);
    printf("############# %d #############\n", toto);
    for (int i = 0; i < toto; i++) {
      printf("%02x ", (unsigned char)dataptr[i]);
    }
    printf("\n");
  }

  size_t r = BIO_ctrl_pending(_ssl_bio_io);
  log_v2::tls()->info("{} bytes to send", r);
  if (r > 0) {
    auto to_send{std::make_shared<io::raw>()};
    to_send->resize(r);
    size_t rr = BIO_read(_ssl_bio_io, to_send->data(), r);
    _substream->write(to_send);

    // FIXME: surely not necessary.
    //_substream->flush();
  }
  return 1;
}
