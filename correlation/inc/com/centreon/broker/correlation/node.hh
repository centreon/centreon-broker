/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_CORRELATION_NODE_HH_
# define CCB_CORRELATION_NODE_HH_

# include <QList>
# include <QSharedPointer>
# include "com/centreon/broker/correlation/issue.hh"

namespace                       com {
  namespace                     centreon {
    namespace                   broker {
      namespace                 correlation {
        /**
         *  @class node node.hh "com/centreon/broker/correlation/node.hh"
         *  @brief Node in the IT graph.
         *
         *  A node is an element of the IT infrastructure graph. It can
         *  either be an host or a service.
         */
        class                   node {
         private:
          void                  _internal_copy(node const& n);

         public:
          QList<node*>          children;
          QList<node*>          depended_by;
          QList<node*>          depends_on;
          unsigned int          host_id;
          QSharedPointer<issue> my_issue;
          QList<node*>          parents;
          unsigned int          service_id;
          time_t                since;
          short                 state;
                                node();
                                node(node const& n);
                                ~node();
          node&                 operator=(node const& n);
        };
      }
    }
  }
}

#endif /* !CCB_CORRELATION_NODE_HH_ */
