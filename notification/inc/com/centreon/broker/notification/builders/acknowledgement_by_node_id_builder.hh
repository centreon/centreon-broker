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

#ifndef CCB_NOTIFICATION_BUILDERS_ACKNOWLEDGEMENT_BY_NODE_ID_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_ACKNOWLEDGEMENT_BY_NODE_ID_BUILDER_HH

#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/acknowledgement.hh"
#  include "com/centreon/broker/notification/builders/acknowledgement_builder.hh"

CCB_BEGIN()

namespace       notification {
  /**
   *  @class acknowledgement_by_node_id_builder acknowledgement_by_node_id_builder.hh "com/centreon/broker/notification/builders/acknowledgement_by_node_id_builder.hh"
   *  @brief Acknowledgement builder by node_id.
   *
   *  This class build a map of acknowledgement by node_id.
   */
  class           acknowledgement_by_node_id_builder
                    : public acknowledgement_builder {
  public:
                  acknowledgement_by_node_id_builder(
                       QMultiHash<node_id, acknowledgement::ptr>& table);

    virtual void  add_ack(node_id id, acknowledgement::ptr ack);

  private:
    QMultiHash<node_id, acknowledgement::ptr>&
                  _table;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_ACKNOWLEDGEMENT_BY_NODE_ID_BUILDER_HH
