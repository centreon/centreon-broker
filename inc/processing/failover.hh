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

#ifndef CCB_PROCESSING_FAILOVER_HH_
# define CCB_PROCESSING_FAILOVER_HH_

# include <QSharedPointer>
# include <QThread>
# include "io/endpoint.hh"

namespace           com {
  namespace         centreon {
    namespace       broker {
      namespace     processing {
        /**
         *  @class failover failover.hh "processing/failover.hh"
         *  @brief Process some endpoint.
         *
         *  Process an endpoint and launch failover if necessary.
         */
        class       failover : public QThread {
          Q_OBJECT;

         public:
          enum      mode {
            input = 1,
            output
          };

         private:
          QSharedPointer<com::centreon::broker::io::endpoint>
                    _endpoint;
          mode      _mode;
                    failover(failover const& f);
          failover& operator=(failover const& f);

         public:
                    failover();
                    ~failover();
          void      run();
          void      set_endpoint(QSharedPointer<com::centreon::broker::io::endpoint> endp,
                      mode m);
        };
      }
    }
  }
}

#endif /* !CCB_PROCESSING_FAILOVER_HH_ */
