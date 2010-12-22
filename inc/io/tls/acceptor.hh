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

#ifndef IO_TLS_ACCEPTOR_HH_
# define IO_TLS_ACCEPTOR_HH_

# include <gnutls/gnutls.h>
# include <memory>
# include "io/acceptor.hh"
# include "io/tls/params.hh"

namespace                io {
  namespace              tls {
    /**
     *  @class acceptor acceptor.hh "io/tls/acceptor.hh"
     *  @brief Perform TLS verification on top of another acceptor.
     *
     *  Within the process of accepting an incoming client, the
     *  IO::TLS::Acceptor class will provide encryption to the lower
     *  stream. Using this class is really simple : build the object,
     *  set some properties and call listen() with the lower acceptor.
     *  Then use it just like you'd use another io::acceptor. Encryption
     *  will be automatically provided on the returned accepted streams.
     */
    class       acceptor : public io::acceptor, public params {
     private:
      std::auto_ptr<io::acceptor>
                _lower;
                acceptor(acceptor const& a);
      acceptor& operator=(acceptor const& a);

     public:
                acceptor();
                ~acceptor();
      stream*   accept();
      void      close();
      void      listen(io::acceptor* a);
    };
  }
}

#endif /* !IO_TLS_ACCEPTOR_HH_ */
