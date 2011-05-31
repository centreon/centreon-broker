/*
** Copyright 2011 Merethis
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

#ifndef CCB_IO_ENDPOINT_HH_
# define CCB_IO_ENDPOINT_HH_

# include <QSharedPointer>
# include "io/stream.hh"

namespace              com {
  namespace            centreon {
    namespace          broker {
      namespace        io {
        /**
         *  @class endpoint endpoint.hh "io/endpoint.hh"
         *  @brief Base class of connectors and acceptors.
         *
         *  Interface that have all methods used by processing threads.
         */
        class          endpoint {
         public:
                       endpoint();
                       endpoint(endpoint const& e);
          virtual      ~endpoint();
          endpoint&    operator=(endpoint const& e);
          virtual void close() = 0;
          virtual QSharedPointer<stream>
                       open() = 0;
        };
      }
    }
  }
}

#endif /* !CCB_IO_ENDPOINT_HH_ */
