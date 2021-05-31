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
#include "com/centreon/broker/tls/internal.hh"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/raw.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tls;
using namespace com::centreon::exceptions;

static long write_cb(BIO *b, int oper, const char *argp,
                        int argi, long argl, long retvalue) {
  stream* s = reinterpret_cast<stream*>(BIO_get_callback_arg(b));
  if (oper == BIO_CB_WRITE) {
    log_v2::tls()->info("before write");
  }
  if (oper == BIO_CB_WRITE | BIO_CB_RETURN) {
    if (argp) {
      log_v2::tls()->info("TLS: sending data to substream, length {}", argi);
      s->write_encrypted(argp, argi);
    }
  }
  return retvalue;
}

/**
 *  @brief Constructor.
 *
 *  When building the stream, you need to provide the session that will
 *  be used to transport encrypted data.
 *
 *  @param[in] sess  TLS session, providing informations on the
 *                   encryption that should be used.
 */
stream::stream(SSL* ssl, bool is_acceptor)
    : io::stream("TLS"),
      _deadline((time_t)-1),
      _ssl(ssl),
      _is_acceptor{is_acceptor},
  /* BIO initializations */
  _inbio{BIO_new(BIO_s_mem())},
  _outbio{BIO_new(BIO_s_mem())} {
  SSL_set_bio(_ssl, _inbio, _outbio);
  BIO_set_callback(_outbio, write_cb);
  BIO_set_callback_arg(_outbio, reinterpret_cast<char*>(this));
}

/**
 * @brief Perform the TLS handshake.
 *
 * @return 0 on success, -1 in case of no data are already there and so this
 * function needs another call, -2 on error.
 */
int stream::handshake() {
  log_v2::tls()->debug("TLS: performing handshake");
  if (_is_acceptor) {
    int ret = SSL_accept(_ssl);
    if (ret < 0) {
      int err = SSL_get_error(_ssl, ret);
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
        return -1;
      else
        return -2;
    }
    return ret;
  }
  else {
    int ret = SSL_connect(_ssl);
    if (ret < 0) {
      int err = SSL_get_error(_ssl, ret);
      if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
        return -1;
      else
        return -2;
    }
    return 0;
  }
}

/**
 *  @brief Destructor.
 *
 *  The destructor will release all acquired ressources that haven't
 *  been released yet.
 */
stream::~stream() noexcept {
  if (_ssl) {
    SSL_shutdown(_ssl);
    SSL_free(_ssl);
    _ssl = nullptr;
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
  d.reset();

  // Read data.
  _deadline = deadline;
  char* pp;
  long size = BIO_get_mem_data(SSL_get_rbio(_ssl), &pp);
  std::shared_ptr<io::raw> buffer{std::make_shared<io::raw>(pp, pp + size)};
  if (size == 0)
    return false;

  d = buffer;
  return true;
}

/**
 *  Read encrypted data from base stream.
 *
 *  @param[out] buffer Output buffer.
 *  @param[in]  size   Maximum size.
 *
 *  @return Number of bytes actually read.
 */
//long long stream::read_encrypted(void* buffer, long long size) {
//  // Read some data.
//  bool timed_out(false);
//  while (_buffer.empty()) {
//    std::shared_ptr<io::data> d;
//    timed_out = !_substream->read(d, _deadline);
//    if (!timed_out && d && d->type() == io::raw::static_type()) {
//      io::raw* r(static_cast<io::raw*>(d.get()));
//      _buffer.reserve(_buffer.size() + r->get_buffer().size());
//      _buffer.insert(_buffer.end(), r->get_buffer().begin(),
//                     r->get_buffer().end());
//      //_buffer.append(r->data(), r->size());
//    } else if (timed_out)
//      break;
//  }
//
//  // Transfer data.
//  uint32_t rb(_buffer.size());
//  if (!rb) {
//    if (timed_out) {
//      gnutls_transport_set_errno(*_session, EAGAIN);
//      return -1;
//    } else {
//      return 0;
//    }
//  } else if (size >= rb) {
//    memcpy(buffer, _buffer.data(), rb);
//    _buffer.clear();
//    return rb;
//  } else {
//    memcpy(buffer, _buffer.data(), size);
//    _buffer.erase(_buffer.begin(), _buffer.begin() + size);
//    //_buffer.remove(0, size);
//    return size;
//  }
//}

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

  // Send data.
  if (d->type() == io::raw::static_type()) {
    io::raw const* packet(static_cast<io::raw const*>(d.get()));
    char const* ptr(packet->const_data());
    int size(packet->size());
    while (size > 0) {
      int ret = SSL_write(_ssl, ptr, size);
      //int ret(gnutls_record_send(*_session, ptr, size));
      if (ret < 0) {
        std::string err{
            fmt::format("TLS: could not send data: {}", tls::err_as_string())};
        log_v2::tls()->error(err);
        throw msg_fmt(err);
      }
      ptr += ret;
      size -= ret;
    }
  }

  return 1;
}

/**
 *  Write encrypted data to base stream.
 *
 *  @param[in] buffer Data to write.
 *  @param[in] size   Size of buffer.
 *
 *  @return Number of bytes written.
 */
long stream::write_encrypted(const char* buffer, long size) {
  std::shared_ptr<io::raw> r(new io::raw);
  std::vector<char> tmp(const_cast<char*>(static_cast<char const*>(buffer)),
                        const_cast<char*>(static_cast<char const*>(buffer)) +
                            static_cast<std::size_t>(size));
  logging::error(logging::low) << "tls write enc: " << size;
  r->get_buffer() = std::move(tmp);
  _substream->write(r);
  _substream->flush();
  return size;
}
