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
#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/downtime.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace       notification {

  class           downtime_by_node_id_builder {
  public:
                  downtime_by_node_id_builder(
                        QMultiHash<node_id, shared_ptr<downtime> >& table);
    virtual       ~downtime_by_node_id_builder() {}

    virtual void  add_downtime(unsigned int downtime_id,
                               shared_ptr<downtime> downtime);

  private:
    QMultiHash<node_id, shared_ptr<downtime> >
                  _table;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DOWNTIME_BY_NODE_ID_BUILDER_HH
