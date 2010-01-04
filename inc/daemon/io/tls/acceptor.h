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

#ifndef IO_TLS_ACCEPTOR_H_
# define IO_TLS_ACCEPTOR_H_

# include <gnutls/gnutls.h> // for gnutls_dh_params_t
# include <memory>          // for auto_ptr
# include "io/acceptor.h"
# include "io/tls/params.h"

namespace                IO
{
  namespace              TLS
  {
    /**
     *  \class Acceptor acceptor.h "io/tls/acceptor.h"
     *  \brief Perform TLS verification on top of another acceptor.
     *
     *  Within the process of accepting an incoming client, the
     *  IO::TLS::Acceptor class will provide encryption to the lower stream.
     *  Using this class is really simple : build the object, set some
     *  properties and call Listen() with the lower acceptor. Then use it just
     *  like you'd use another IO::Acceptor. Encryption will be automatically
     *  provided on the returned accepted streams.
     */
    class                Acceptor : public IO::Acceptor, public Params
    {
     private:
      std::auto_ptr<IO::Acceptor>
                         lower_;
                         Acceptor(const Acceptor& acceptor);
      Acceptor&          operator=(const Acceptor& acceptor);

     public:
                         Acceptor();
                         ~Acceptor();
      Stream*            Accept();
      void               Close();
      void               Listen(Acceptor* acceptor);
    };
  }
}

#endif /* !IO_TLS_ACCEPTOR_H_ */
