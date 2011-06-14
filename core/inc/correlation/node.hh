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

#ifndef CORRELATION_NODE_HH_
# define CORRELATION_NODE_HH_

# include <list>
# include "events/host_status.hh"
# include "events/issue.hh"

namespace            correlation {
  /**
   *  @class node node.hh "correlation/node.hh"
   *  @brief Node in the IT graph.
   *
   *  A node is an element of the IT infrastructure graph. It can either
   *  be an host or a service.
   */
  class              node {
   private:
    void             _internal_copy(node const& n);

   public:
    std::list<node*> children;
    std::list<node*> depended_by;
    std::list<node*> depends_on;
    int              host_id;
    events::issue*   issue;
    std::list<node*> parents;
    int              service_id;
    time_t           since;
    short            state;
                     node();
                     node(node const& n);
                     ~node();
    node&            operator=(node const& n);
    node&            operator<<(events::host_status const& hs);
  };
}

#endif /* !CORRELATION_NODE_HH_ */
