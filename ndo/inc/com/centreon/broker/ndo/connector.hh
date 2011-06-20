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

#ifndef CCB_NDO_CONNECTOR_HH_
# define CCB_NDO_CONNECTOR_HH_

# include "com/centreon/broker/io/endpoint.hh"

namespace            com {
  namespace          centreon {
    namespace        broker {
      namespace      ndo {
        /**
         *  @class connector connector.hh "com/centreon/broker/ndo/connector.hh"
         *  @brief NDO connector.
         *
         *  Initiate NDO connections.
         */
        class        connector : public io::endpoint {
         private:
          bool       _is_in;
          bool       _is_out;

         public:
                     connector(bool is_in, bool is_out);
                     connector(connector const& c);
                     ~connector();
          connector& operator=(connector const& c);
          void       close();
          QSharedPointer<io::stream>
                     open();
        };
      }
    }
  }
}

#endif /* !CCB_NDO_CONNECTOR_HH_ */
