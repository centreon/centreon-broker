/*
** Copyright 2011-2014 Merethis
**
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

#ifndef CCB_NOTIFICATION_BUILDERS_NODE_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_NODE_BUILDER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class node_builder node_builder.hh "com/centreon/broker/notification/builders/node_builder.hh"
   *  @brief Node builder interface.
   *
   *  This interface define what methods node builders need to implement.
   */
  class          node_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual      ~node_builder() {}

    virtual void add_node(node::ptr node) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_NODE_BUILDER_HH
