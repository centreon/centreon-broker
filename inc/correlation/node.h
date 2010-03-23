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

#ifndef CORRELATION_NODE_H_
# define CORRELATION_NODE_H_

# include <list>
# include <memory> // for auto_ptr
# include "events/host_status.h"
# include "events/issue.h"

namespace            Correlation
{
  /**
   *  \class Node node.h "correlation/node.h"
   *  \brief Node in the IT graph.
   *
   *  A node is an element of the IT infrastructure graph. It can either be an
   *  host or a service.
   */
  class              Node
  {
   private:
    void             InternalCopy(const Node& node);

   public:
    std::list<Node*> children;
    std::list<Node*> depended_by;
    std::list<Node*> depends_on;
    int              host_id;
    std::auto_ptr<Events::Issue>
                     issue;
    std::list<Node*> parents;
    int              service_id;
    short            state;
                     Node();
                     Node(const Node& node);
                     ~Node();
    Node&            operator=(const Node& node);
    Node&            operator<<(const Events::HostStatus& hs);
  };
}

#endif /* !CORRELATION_NODE_H_ */
