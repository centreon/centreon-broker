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

#ifndef CCB_RRD_CONNECTOR_HH_
# define CCB_RRD_CONNECTOR_HH_

# include <QString>
# include "io/connector.hh"

namespace            com {
  namespace          centreon {
    namespace        broker {
      namespace      rrd {
        /**
         *  @class connector connector.hh "rrd/connector.hh"
         *  @brief RRD connector.
         *
         *  Generate an RRD stream that will write files.
         */
        class        connector : public com::centreon::broker::io::connector {
         private:
          QString    _rrd_path;

         public:
                     connector();
                     connector(connector const& c);
                     ~connector();
          connector& operator=(connector const& c);
          void       close();
          QSharedPointer<com::centreon::broker::io::stream>
                     open();
          void       set_path(QString const& rrd_path);
        };
      }
    }
  }
}

#endif /* !CCB_RRD_CONNECTOR_HH_ */
