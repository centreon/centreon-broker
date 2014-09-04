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

#ifndef CCB_NOTIFICATION_BUILDERS_NODE_SET_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_NODE_SET_BUILDER_HH

#  include <QSet>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/builders/node_builder.hh"

CCB_BEGIN()

namespace           notification {
  /**
   *  @class node_set_builder node_set_builder.hh "com/centreon/broker/notification/builders/node_set_builder.hh"
   *  @brief Build the set of node id.
   *
   *  This class build the set of all the node id of the db.
   */
  class             node_set_builder : public node_builder{
  public:
                    node_set_builder(QSet<node_id>& set);
    virtual         ~node_set_builder() {}

    virtual void    add_node(node::ptr node);

  private:
                    node_set_builder();

    QSet<node_id>&  _set;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_NODE_SET_BUILDER_HH
