/*
** Copyright 2009-2013, 2021 Centreon
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

#ifndef CCB_TLS_STREAM_HH
#define CCB_TLS_STREAM_HH

#include <gnutls/gnutls.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <string.h>
#include <string>
#include <vector>

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/misc/buffer.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tls {
/**
 *  @class stream stream.hh "com/centreon/broker/tls/stream.hh"
 *  @brief TLS wrapper of an underlying stream.
 *
 *  The TLS stream class wraps a lower layer stream and provides
 *  encryption (and optionnally compression) over this stream. Those
 *  functionnality are provided using the OpenSSL library.
 */
class stream : public io::stream {
  enum ssl_action { ssl_handshake, ssl_write, ssl_read };
  bool _handshake_done;
  int32_t _pending;
  int32_t _ack;
  time_t _deadline;
  SSL* _ssl;
  BIO* _bio;
  BIO* _bio_io;

  misc::buffer _rbuf;
  misc::buffer _wbuf;

  void _do_stream();
  bool _do_read(int timeout);
  void _manage_stream(int r, ssl_action action);

 public:
  stream(SSL* ssl, BIO* bio, BIO* bio_io);
  ~stream();
  stream(const stream&) = delete;
  stream& operator=(const stream&) = delete;
  void handshake();
  bool read(std::shared_ptr<io::data>& d, time_t deadline) override;
  int32_t write(const std::shared_ptr<io::data>& d) override;
  int32_t flush() override;
  int32_t stop() override;
};
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_STREAM_HH
