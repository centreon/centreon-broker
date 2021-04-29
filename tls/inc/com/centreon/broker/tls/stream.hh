/*
** Copyright 2009-2013 Centreon
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

#include <vector>

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tls {
/**
 *  @class stream stream.hh "com/centreon/broker/tls/stream.hh"
 *  @brief TLS wrapper of an underlying stream.
 *
 *  The TLS stream class wraps a lower layer stream and provides
 *  encryption (and optionnally compression) over this stream. Those
 *  functionnality are provided using the GNU TLS library
 *  (http://www.gnu.org/software/gnutls).
 */
class stream : public io::stream {
  std::vector<char> _buffer;
  time_t _deadline;
  gnutls_session_t* _session;

 public:
  stream(gnutls_session_t* session);
  ~stream();
  stream(const stream&) = delete;
  stream& operator=(const stream&) = delete;
  bool read(std::shared_ptr<io::data>& d, time_t deadline) override;
  long long read_encrypted(void* buffer, long long size);
  int32_t write(std::shared_ptr<io::data> const& d) override;
  int32_t stop() override { return 0; }
  long long write_encrypted(void const* buffer, long long size);
};
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_STREAM_HH
