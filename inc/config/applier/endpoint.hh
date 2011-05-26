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

#ifndef CCB_CONFIG_APPLIER_ENDPOINT_HH_
# define CCB_CONFIG_APPLIER_ENDPOINT_HH_

# include <QList>
# include "config/endpoint.hh"

namespace                    com {
  namespace                  centreon {
    namespace                broker {
      namespace              config {
        namespace            applier {
          /**
           *  @class endpoint endpoint.hh "config/applier/endpoint.hh"
           *  @brief Apply the configuration of input and output endpoints.
           *
           *  Apply the configuration of the input and output endpoints.
           */
          class              endpoint {
           private:
                             endpoint();
                             endpoint(endpoint const& e);
            endpoint&        operator=(endpoint const& e);

           public:
                             ~endpoint();
            void             apply(QList<com::centreon::broker::config::endpoint> const& inputs,
                               QList<com::centreon::broker::config::endpoint> const& outputs);
            static endpoint& instance();
          };
        }
      }
    }
  }
}

#endif /* !CCB_CONFIG_APPLIER_ENDPOINT_HH_ */
