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

#ifndef IO_TLS_CONNECTOR_H_
# define IO_TLS_CONNECTOR_H_

# include <memory>
# include "io/tls/params.h"
# include "io/tls/stream.h"

namespace        IO
{
  namespace      TLS
  {
    /**
     *  \class Connector connector.h "io/tls/connector.h"
     *  \brief Connect to a TLS peer.
     *
     *  Use a connected IO::Stream to connect to a TLS peer.
     */
    class        Connector : public Params, public Stream
    {
     private:
                 Connector(const Connector& connector);
      Connector& operator=(const Connector& connector);

     public:
                 Connector(IO::Stream* stream);
                 ~Connector();
      void       Connect();
    };
  }
}

#endif /* !IO_TLS_CONNECTOR_H_ */
