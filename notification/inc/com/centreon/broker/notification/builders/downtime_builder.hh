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

#ifndef CCB_NOTIFICATION_BUILDERS_DOWNTIME_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_DOWNTIME_BUILDER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/downtime.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class downtime_builder downtime_builder.hh "com/centreon/broker/notification/builders/downtime_builder.hh"
   *  @brief Downtime builder interface.
   *
   *  This interface define what methods downtime builders need to implement.
   */
  class           downtime_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual       ~downtime_builder() {}

    /**
     *  Add a downtime to the builder.
     *
     *  @param[in] id The id of the downtime.
     *  @param[in] downtime The downtime.
     */
    virtual void  add_downtime(
                    unsigned int downtime_id,
                    objects::downtime::ptr downtime) {
      (void)downtime_id;
      (void)downtime;
    }

    /**
     *  Connect a downtime to a node_id.
     *
     *  @param[in] downtime  The id of the downtime.
     *  @param[in] node_id   The node id associated with this downtime.
     */
    virtual void  connect_downtime_to_node(
                    unsigned int downtime_id,
                    objects::node_id node_id) {
      (void)downtime_id;
      (void)node_id;
    }
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DOWNTIME_BUILDER_HH
