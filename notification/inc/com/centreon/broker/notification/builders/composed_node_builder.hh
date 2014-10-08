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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_NODE_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_COMPOSED_NODE_BUILDER_HH

#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node.hh"
#  include "com/centreon/broker/notification/builders/node_builder.hh"
#  include "com/centreon/broker/notification/builders/composed_builder.hh"

CCB_BEGIN()

namespace       notification {
  /**
   *  @class composed_node_builder composed_node_builder.hh "com/centreon/broker/notification/builders/composed_node_builder.hh"
   *  @brief Composed node builder.
   *
   *  This class forward its method call to several other builders.
   */
  class           composed_node_builder : public composed_builder<node_builder> {
  public:
                  composed_node_builder();

    virtual void  add_node(objects::node::ptr node);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_COMPOSED_NODE_BUILDER_HH
