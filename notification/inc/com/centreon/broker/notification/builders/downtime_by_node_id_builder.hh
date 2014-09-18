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

#ifndef CCB_NOTIFICATION_BUILDERS_DOWNTIME_BY_NODE_ID_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_DOWNTIME_BY_NODE_ID_BUILDER_HH

#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/downtime.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/builders/downtime_builder.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class downtime_by_node_id_builder downtime_by_node_id_builder.hh "com/centreon/broker/notification/builders/downtime_by_node_id_builder.hh"
   *  @brief Downtime builder by node id.
   *
   *  This class build a map of downtimes by their node id.
   */
  class           downtime_by_node_id_builder : public downtime_builder {
  public:
                  downtime_by_node_id_builder(
                        QMultiHash<objects::node_id,
                                   objects::downtime::ptr>& table);

    virtual void  add_downtime(unsigned int downtime_id,
                               objects::downtime::ptr downtime);

  private:
    QMultiHash<objects::node_id, objects::downtime::ptr>
                  _table;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DOWNTIME_BY_NODE_ID_BUILDER_HH
