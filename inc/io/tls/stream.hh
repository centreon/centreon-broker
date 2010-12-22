/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef IO_TLS_STREAM_HH_
# define IO_TLS_STREAM_HH_

# include <gnutls/gnutls.h>
# include <memory>
# include "io/stream.hh"

namespace               io {
  namespace             tls {
    /**
     *  @class stream stream.hh "io/tls/stream.hh"
     *  @brief TLS wrapper of an underlying stream.
     *
     *  The io::tls::stream class wraps a lower layer stream and
     *  provides encryption (and optionnally compression) over this
     *  stream. Those functionnality are provided using the GNU TLS
     *  library (http://www.gnu.org/software/gnutls). io::tls::stream
     *  can be used on every io::stream object.
     */
    class               stream : public io::stream {
     private:
      std::auto_ptr<io::stream>
                        _lower;
                        stream(stream const& s);
      stream&           operator=(stream const& s);

     protected:
      gnutls_session_t* session;

     public:
                        stream(io::stream* lower, gnutls_session_t* session);
      virtual           ~stream();
      void              close();
      unsigned int      receive(void* buffer, unsigned int size);
      unsigned int      send(void const* buffer, unsigned int size);
    };
  }
}

#endif /* !IO_TLS_STREAM_HH_ */
