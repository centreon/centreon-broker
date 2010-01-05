/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef IO_TLS_STREAM_H_
# define IO_TLS_STREAM_H_

# include <gnutls/gnutls.h> // for gnutls_session_t
# include <memory>          // for auto_ptr
# include "io/stream.h"

namespace               IO
{
  namespace             TLS
  {
    /**
     *  \class Stream stream.h "io/tls/stream.h"
     *  \brief TLS wrapper of an underlying stream.
     *
     *  The IO::TLS::Stream class wraps a lower layer stream and provides
     *  encryption (and optionnally compression) over this stream. Those
     *  functionnality are provided using the GNU TLS library
     *  (http://www.gnu.org/software/gnutls). IO::TLS::Stream can be used on
     *  every IO::Stream object.
     */
    class               Stream : public IO::Stream
    {
     private:
      std::auto_ptr<IO::Stream>
                        lower_;
                        Stream(const Stream& stream);
      Stream&           operator=(const Stream& stream);

     protected:
      gnutls_session_t* session;

     public:
                        Stream(IO::Stream* lower, gnutls_session_t* session);
      virtual           ~Stream();
      void              Close();
      unsigned int      Receive(void* buffer, unsigned int size);
      unsigned int      Send(const void* buffer, unsigned int size);
    };
  }
}

#endif /* !IO_TLS_STREAM_H_ */
