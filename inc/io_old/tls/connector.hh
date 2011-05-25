/*
** Copyright 2009-2011 MERETHIS
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

#ifndef IO_TLS_CONNECTOR_HH_
# define IO_TLS_CONNECTOR_HH_

# include <memory>
# include "io/tls/params.hh"
# include "io/tls/stream.hh"

namespace        io {
  namespace      tls {
    /**
     *  \class connector connector.hh "io/tls/connector.hh"
     *  \brief Connect to a TLS peer.
     *
     *  Use a connected io::stream to connect to a TLS peer.
     */
    class        connector : public params, public stream {
     private:
                 connector(connector const& c);
      connector& operator=(connector const& c);

     public:
                 connector(io::stream* s);
                 ~connector();
      void       connect();
    };
  }
}

#endif /* !IO_TLS_CONNECTOR_HH_ */
