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

    /**
     *  Add a node to the builder.
     *
     *  @param[in] node The node.
     */
    virtual void add_node(objects::node::ptr node) {}

    /**
     *  Connect a node to its servicegroup.
     *
     *  @param[in] id         The id of the node.
     *  @param[in] group_id   The id of the group.
     */
    virtual void connect_node_servicegroup(objects::node_id id,
                                           unsigned int group_id) {}

    /**
     *  Connect a node to its hostgroup.
     *
     *  @param[in] id         The id of the node.
     *  @param[in] group_id   The id of the group.
     */
    virtual void connect_node_hostgroup(objects::node_id id,
                                        unsigned int group_id) {}

    /**
     *  Connect a hostgroup to its servicegroup.
     *
     *  @param[in] hostgroup_id     The id of the hostgroup.
     *  @param[in] servicegroup_id  The id of the servicegroup.
     */
    virtual void connect_hostgroup_servicegroup(
                    unsigned int hostgroup_id,
                    unsigned int servicegroup_id) {}

    /**
     *  Connect a hostgroup to its parent hostgroup.
     *
     *  @param[in] hostgroup_id         The id of the hostgroup.
     *  @param[in] parent_hostgroup_id  The id of the servicegroup.
     */
    virtual void connect_hostgroup_parent_hostgroup(
                    unsigned int hostgroup_id,
                    unsigned int parent_hostgroup_id) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_NODE_BUILDER_HH
