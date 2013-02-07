/*
** Copyright 2009-2013 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
    void              write(misc::shared_ptr<io::data> const& d);
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
