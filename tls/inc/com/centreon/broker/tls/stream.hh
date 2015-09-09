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
#  define CCB_TLS_STREAM_HH

#  include <gnutls/gnutls.h>
#  include <QByteArray>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             tls {
  /**
   *  @class stream stream.hh "com/centreon/broker/tls/stream.hh"
   *  @brief TLS wrapper of an underlying stream.
   *
   *  The TLS stream class wraps a lower layer stream and provides
   *  encryption (and optionnally compression) over this stream. Those
   *  functionnality are provided using the GNU TLS library
   *  (http://www.gnu.org/software/gnutls).
   */
  class               stream : public io::stream {
  public:
                      stream(gnutls_session_t* session);
                      ~stream();
    void              process(bool in = false, bool out = false);
    void              read(misc::shared_ptr<io::data>& d);
    unsigned int      read_encrypted(void* buffer, unsigned int size);
    unsigned int      write(misc::shared_ptr<io::data> const& d);
    unsigned int      write_encrypted(
                        void const* buffer,
                        unsigned int size);

  private:
                      stream(stream const& s);
    stream&           operator=(stream const& s);

    QByteArray        _buffer;
    bool              _process_in;
    bool              _process_out;
    gnutls_session_t* _session;
  };
}

CCB_END()

#endif // !CCB_TLS_STREAM_HH
