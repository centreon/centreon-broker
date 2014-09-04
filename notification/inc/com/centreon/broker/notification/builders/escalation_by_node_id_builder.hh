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

#ifndef CCB_NOTIFICATION_BUILDERS_ESCALATION_BY_NODE_ID_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_ESCALATION_BY_NODE_ID_BUILDER_HH

#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/escalation.hh"
#  include "com/centreon/broker/notification/builders/escalation_builder.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class escalation_by_node_id_builder escalation_by_node_id_builder.hh "com/centreon/broker/notification/builders/escalation_by_node_id_builder.hh"
   *  @brief Escalation builder by node id.
   *
   *  This class build a map of escalations by their node id.
   */
  class           escalation_by_node_id_builder : public escalation_builder {
  public:
                  escalation_by_node_id_builder(
                      QMultiHash<node_id, escalation::ptr>& table);

    virtual void  add_escalation(unsigned int id,
                                 escalation::ptr esc);
    virtual void  connect_escalation_node_id(unsigned int esc_id,
                                             node_id id);
  private:
    QHash<unsigned int, escalation::ptr>
                  _cache;
    QMultiHash<node_id, escalation::ptr>&
                  _table;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_ESCALATION_BY_NODE_ID_BUILDER_HH
