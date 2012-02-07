/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_CONFIG_ENDPOINT_HH_
# define CCB_CONFIG_ENDPOINT_HH_

# include <memory>
# include <QMap>
# include <QString>
# include <time.h>

namespace                          com {
  namespace                        centreon {
    namespace                      broker {
      namespace                    config {
        /**
         *  @class endpoint endpoint.hh "com/centreon/broker/config/endpoint.hh"
         *  @brief Hold configuration of an endpoint.
         *
         *  An endpoint is an external source or destination for events.
         *  This can either be an XML stream, a database, a file, ...
         *  This class holds the configuration of an endpoint.
         */
        class                      endpoint {
         private:
          void                     _internal_copy(endpoint const& e);

         public:
          time_t                   buffering_timeout;
          QString                  failover;          
          std::auto_ptr<endpoint>  failover_config;
          QString                  name;
          QMap<QString, QString>   params;
          time_t                   retry_interval;
          QString                  type;
                                   endpoint();
                                   endpoint(endpoint const& e);
                                   ~endpoint();
          endpoint&                operator=(endpoint const& e);
          bool                     operator==(endpoint const& e) const;
          bool                     operator!=(endpoint const& e) const;
          bool                     operator<(endpoint const& e) const;
        };
      }
    }
  }
}

#endif /* !CCB_CONFIG_ENDPOINT_HH_ */
