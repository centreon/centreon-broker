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

#ifndef CCB_NOTIFICATION_BUILDERS_NODEGROUP_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_NODEGROUP_BUILDER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/nodegroup.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class nodegroup_builder nodegroup_builder.hh "com/centreon/broker/notification/builders/nodegroup_builder.hh"
   *  @brief Nodegroup builder interface.
   *
   *  This interface define what methods nodegroup builders need to implement.
   */
  class          nodegroup_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual      ~nodegroup_builder() {}

    /**
     *  Add a nodegroup to the builder.
     *
     *  @param[in] id   The nodegroup id.
     *  @param[in] ndg  The nodegroup.
     */
    virtual void add_nodegroup(
                   objects::node_id id,
                   objects::nodegroup::ptr ndg) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_NODEGROUP_BUILDER_HH
