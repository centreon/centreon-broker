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
#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/opensslconf.h>
#include <openssl/ssl.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"

#include "com/centreon/broker/neb/service.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::exceptions;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  @brief Constructor.
 *
 *  When building the stream, you need to provide the session that will
 *  be used to transport encrypted data.
 *
 *  @param[in] sess  TLS session, providing informations on the
 *                   encryption that should be used.
 */
stream::stream(SSL* sess, BIO* rbio, BIO* wbio)
    : io::stream("TLS"),
      _deadline((time_t)-1),
      _session(sess),
      _rbio(rbio),
      _wbio(wbio) {}

/**
 *  @brief Destructor.
 *
 *  The destructor will release all acquired ressources that haven't
 *  been released yet.
 */
stream::~stream() {
  if (_session) {
    try {
      _deadline = time(nullptr) + 30;  // XXX : use connection timeout

      SSL_free(_session);
      delete (_session);
      _session = nullptr;
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
  // Clear existing content.
  std::cout << "TLS: on read stream : 0" << std::endl;
  d.reset();
  std::shared_ptr<io::data> s;
  std::cout << "TLS: on read stream : 1" << std::endl;
  _substream->read(s, deadline);
  std::cout << "TLS: on read stream : 2" << std::endl;
  io::raw* packet(static_cast<io::raw*>(s.get()));
  char* ptr(packet->data());
  int size(packet->size());
  std::cout << "TLS: on read stream : 3" << std::endl;
  int rets(BIO_write(_rbio, ptr, size));
  std::cout << "TLS: on read stream : 4" << std::endl;
  if (rets > 0) {
    int ret(SSL_read(_session, ptr, size));
    std::cout << "TLS: on read stream : 5" << std::endl;
    if (ret < 0) {
      if ((ret != GNUTLS_E_INTERRUPTED) && (ret != GNUTLS_E_AGAIN)) {
        std::cout << "TLS: could not receive data: " << std::endl;

      } else
        return false;
    } else if (ret) {
      std::shared_ptr<io::raw> r(new io::raw);
      std::cout << "tls read enc: " << size << std::endl;
      std::vector<char> tmp(const_cast<char*>(static_cast<char const*>(ptr)),
                            const_cast<char*>(static_cast<char const*>(ptr)) +
                                static_cast<std::size_t>(size));
      std::cout << "tls read enc: " << size << std::endl;
      r->get_buffer() = tmp;
      d = r;
      return true;
    } else {
      std::cout << "TLS session is terminated " << std::endl;
    }
  } else {
    std::cout << "TLS: on read stream rateeeee " << std::endl;
  }
  return false;
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
int stream::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, get_name()))
    return 1;
  std::cout << "tls write 0" << std::endl;
  // Send data.
  if (d->type() == io::raw::static_type()) {
    io::raw const* packet(static_cast<io::raw const*>(d.get()));
    char const* ptr(packet->const_data());
    int size(packet->size());
    int ret;
    std::cout << "tls write 1 " << size << std::endl;

    ret = SSL_write(_session, ptr, size);
    std::cout << "tls write 2 ret " << ret << std::endl;
    std::cout << "tls write 2 error " << SSL_get_error(_session, ret)
              << std::endl;
    BIO_printf(_wbio, "ERROR\n");
    (void)BIO_flush(_wbio);
    ERR_print_errors(_wbio);
    if (ret > 0) {
      std::cout << "tls write 3" << std::endl;
      /* take the output of the SSL object and queue it for socket write */
      io::raw* packets(static_cast<io::raw*>(d.get()));
      char* ptrs(packets->data());
      int sizes(packets->size());
      ret = BIO_read(_wbio, ptrs, sizes);
      std::cout << "tls write 4" << std::endl;
      if (ret > 0) {
        std::cout << "tls bio write enc: " << std::endl;
        std::shared_ptr<io::raw> r(new io::raw);
        std::vector<char> tmp(const_cast<char*>(static_cast<char*>(ptrs)),
                              const_cast<char*>(static_cast<char*>(ptrs)) +
                                  static_cast<std::size_t>(sizes));
        std::cout << "tls write enc: " << sizes << std::endl;
        r->get_buffer() = tmp;
        std::cout << "tls write enc: " << _substream->get_name() << std::endl;
        _substream->write(r);
        std::cout << "tls write enc: 4" << std::endl;
        _substream->flush();

      } else if (!BIO_should_retry(_wbio)) {
        std::cout << "tls bio write ratééé " << std::endl;
        return 0;
      }
    } else {
      std::cout << "tls write ratééé " << std::endl;
    }
  }

  return 1;
}
