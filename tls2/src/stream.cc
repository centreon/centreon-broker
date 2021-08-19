/*
** Copyright 2009-2017, 2020-2021 Centreon
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

#include "com/centreon/broker/tls2/stream.hh"

#include <cstdlib>
#include <thread>

#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls2;
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
stream::stream(SSL* ssl, BIO* bio, BIO* bio_io, bool server)
    : io::stream("TLS"),
      _server(server),
      _handshake_done{false},
      _deadline((time_t)-1),
      _ssl(ssl),
      _bio(bio),
      _bio_io(bio_io) {}

/**
 *  @brief Destructor.
 *
 *  The destructor will release all acquired ressources that haven't
 *  been released yet.
 */
stream::~stream() {
  SSL_shutdown(_ssl);
  // SSL_free(_ssl);
  BIO_free(_bio);
  BIO_free(_bio_io);
}

/**
 * @brief Do SSL handshake.
 *
 */
void stream::handshake() {
  int r;
  while ((r = SSL_do_handshake(_ssl)) != 1)
    _manage_stream_error(r, ssl_handshake);
  log_v2::tls()->warn("{} Handshake done",
                         _server ? "SERVER" : "CLIENT");
  _handshake_done = true;
}

/**
 * @brief Read data available for the substream and attempt to treat them within
 * the SSL filter.
 *
 * @param timeout The timeout provided to this thread to read data.
 *
 * @return true if data have been read, false otherwise.
 */
bool stream::_do_read(int timeout) {
  log_v2::tls()->trace("{}: do_read()", _server ? "SERVER":"CLIENT");
  std::shared_ptr<io::data> d;
  bool no_timeout = _substream->read(d, timeout);
  if (no_timeout) {
    io::raw* packet = static_cast<io::raw*>(d.get());
    _rbuf.push(packet->get_buffer());
    int sz = packet->size();
    log_v2::tls()->trace("do read {:x} {} bytes: Encrypted data {}",
                         pthread_self(), sz,
                         misc::string::from_buffer(packet->data(), sz));
  }
  if (_rbuf.size() > 0) {
    int r = BIO_ctrl_get_write_guarantee(_bio_io);
    no_timeout = true;
    std::vector<char> v{_rbuf.pop(r)};
    int ss = BIO_write(_bio_io, v.data(), v.size());
    assert(ss == v.size());
  }
  return no_timeout;
}

void stream::_do_stream() {
  log_v2::tls()->trace("{}: do_stream()", _server ? "SERVER":"CLIENT");
  int sz;
  bool something_done = false;
  while ((sz = BIO_ctrl_pending(_bio_io)) > 0) {
    something_done = true;
    log_v2::tls()->trace("{}: do_stream() -> substream write", _server ? "SERVER":"CLIENT");
    char* dataptr;
    sz = BIO_nread(_bio_io, &dataptr, sz);
    log_v2::tls()->trace("{}: do_stream() -> BIO_nread {} bytes", _server ? "SERVER":"CLIENT", sz);
    auto packet = std::make_shared<io::raw>();
    packet->get_buffer().insert(packet->get_buffer().end(), dataptr,
                                dataptr + sz);
    log_v2::tls()->trace("TLS: {} {:x} Sending SSL buffer '{}' of {} bytes",
                         _server ? "SERVER" : "CLIENT", pthread_self(),
                         misc::string::from_buffer(dataptr, sz), sz);
    _substream->write(packet);
  }

  while ((sz = BIO_ctrl_get_read_request(_bio_io)) > 0) {
    log_v2::tls()->trace("{}: do_stream() -> substream read {} bytes wanted", _server ? "SERVER":"CLIENT", sz);
    something_done = true;
    std::shared_ptr<io::data> d;
    bool no_timeout = _substream->read(d, 0);
    if (no_timeout) {
      io::raw* packet = static_cast<io::raw*>(d.get());
      _rbuf.push(packet->get_buffer());
      log_v2::tls()->trace("TLS: {} {:x} Receiving SSL buffer '{}' of {} bytes",
                           _server ? "SERVER" : "CLIENT", pthread_self(),
                           misc::string::from_buffer(packet->data(), sz), sz);
    }
    if (_rbuf.size() > 0) {
      int s = std::min(_rbuf.size(), static_cast<size_t>(sz));
      log_v2::tls()->trace("rbuf size {} ; bytes to pop {}", _rbuf.size(), s);
      std::vector<char> v{_rbuf.pop(s)};
      log_v2::tls()->trace("new rbuf size {}", _rbuf.size());
      int ss = BIO_write(_bio_io, v.data(), s);
      log_v2::tls()->trace("{}: do_stream() -> BIO_write {} bytes", _server ? "SERVER":"CLIENT", ss);
    } else
      break;
  }
  if (!something_done) {
    _do_read(0);
  }
}

/**
 * @brief In a stream we have only the half of the SSL exchange. We read/write
 *        data thanks to SSL_write/SSL_read, they are encrypted and we can get
 *        them back as encrypted from the _bio_io, but we still need to send
 *        them to the other side (or to receive them). This is why of this
 *        _manage_stream_error() function. It gets the return value of
 *        SSL_do_handshake(), SSL_write() or SSL_read() as parameter and does
 *        what is needed.
 *
 * @param r The return value of an SSL_...() functions.
 */
void stream::_manage_stream_error(int r, ssl_action action) {
  int err = SSL_get_error(_ssl, r);
  switch (err) {
    case SSL_ERROR_WANT_READ:
      log_v2::tls()->trace("TLS: {} {} WANT READ",
                           _server ? "SERVER" : "CLIENT",
                           action == ssl_handshake ? "HANDSHAKE" : "");
      _do_stream();
      break;
    case SSL_ERROR_WANT_WRITE:
      log_v2::tls()->trace("TLS: {} {} WANT WRITE",
                           _server ? "SERVER" : "CLIENT",
                           action == ssl_handshake ? "HANDSHAKE" : "");
      _do_stream();
      break;
    case SSL_ERROR_NONE:
      log_v2::tls()->info("SSL_ERROR_NONE");
      if (action == ssl_handshake) {
        _handshake_done = true;
        log_v2::tls()->info(
            "Encryption protocol '{}' configured with '{}' cipher",
            SSL_get_version(_ssl), SSL_get_cipher_name(_ssl));
      }
      _do_stream();
      break;
    default: {
      log_v2::tls()->error("TLS: SSL error: {}",
                           ERR_reason_error_string(ERR_get_error()));
      throw msg_fmt("TLS: SSL error. See logs");
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
  bool retval;
  log_v2::tls()->trace(
      "TLS: {} {:x} read ==> {}",
      _server ? "SERVER" : "CLIENT", pthread_self(),
      SSL_state_string_long(_ssl));
  log_v2::tls()->trace("read {} : rbuf size {}", _server ? "SERVER" : "CLIENT",
                       _rbuf.size());
  try {
    retval = _do_read(deadline);
    log_v2::tls()->trace("_do_read returned {}", retval);
    if (!_handshake_done) {
      retval = false;
      log_v2::tls()->trace("TLS: {} {:x} waiting ==> {}",
                           _server ? "SERVER" : "CLIENT",
                           pthread_self(),
                           SSL_state_string_long(_ssl));
      handshake();
      _handshake_done = true;
    } else {
      //log_v2::tls()->trace("TLS: want to read!");
      char v[4096];
      int r = SSL_read(_ssl, v, sizeof(v));
      if (r > 0) {
        log_v2::tls()->trace("TLS: {} {:x} SSL read {} bytes: '{}'",
                             _server ? "SERVER" : "CLIENT", pthread_self(), r,
                             misc::string::from_buffer(v, r));
        auto packet = std::make_shared<io::raw>(v, r);
        d = packet;
        assert(d);
        retval = true;
      } else {
        retval = false;
        _manage_stream_error(r, ssl_read);
      }
      assert(d || !retval);
    }
  } catch (const std::exception& e) {
    log_v2::tls()->error("TLS session is terminated");
    throw msg_fmt("TLS session is terminated");
  }

  assert(d || !retval);
  return retval;
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

  io::raw* packet{static_cast<io::raw*>(d.get())};
  log_v2::tls()->trace(
      "TLS: {} {:x} write '{}' of size {} ==> {}",
      _server ? "SERVER" : "CLIENT", pthread_self(),
      misc::string::from_buffer(packet->data(), packet->size()), packet->size(),
      SSL_state_string_long(_ssl));
  _wbuf.push(packet->get_buffer());

  if (_handshake_done) {
    auto v{_wbuf.front()};
    int r = SSL_write(_ssl, v.first, v.second);
    if (r > 0) {
      log_v2::tls()->warn("SSL_write {}", r);
      if (r == v.second)
        _wbuf.pop();
      else {
        // FIXME DBR: is this a current behaviour?
        assert(1 == 0);
      }
      _do_stream();
    }
    else
      _manage_stream_error(r, ssl_write);
  } else {
    log_v2::tls()->trace("TLS: {} {:x} waiting - {}",
                         _server ? "SERVER" : "CLIENT", pthread_self(),
                         SSL_state_string_long(_ssl));
    handshake();
    _handshake_done = true;
  }
  return 1;
}

int32_t stream::flush() {
  if (_wbuf.empty())
    return 0;

  if (_handshake_done) {
    auto v{_wbuf.front()};
    int r = SSL_write(_ssl, v.first, v.second);
    if (r == 1)
      _wbuf.pop();
    else
      _manage_stream_error(r, ssl_write);
  } else {
    log_v2::tls()->trace("TLS: {} {:x} waiting - {}",
                         _server ? "SERVER" : "CLIENT", pthread_self(),
                         SSL_state_string_long(_ssl));
    handshake();
  }
  return 0;
}

int32_t stream::stop() {
  return flush();
}
