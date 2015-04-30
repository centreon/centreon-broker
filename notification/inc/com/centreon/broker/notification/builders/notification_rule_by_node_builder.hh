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

#ifndef CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_NODE_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_NODE_BUILDER_HH

#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/notification_rule_builder.hh"

CCB_BEGIN()

namespace           notification {
  /**
   *  @class notification_method_by_node_builder notification_method_by_node_builder.hh "com/centreon/broker/notification/builders/notification_method_by_node_builder.hh"
   */
  class             notification_rule_by_node_builder
                      : public notification_rule_builder {
  public:
                    notification_rule_by_node_builder(
                      QMultiHash<
                        objects::node_id,
                        objects::notification_rule::ptr>& map);

    virtual void    add_rule(
                      unsigned int rule_id,
                      objects::notification_rule::ptr rule);
  private:
                    notification_rule_by_node_builder();

    QMultiHash<objects::node_id, objects::notification_rule::ptr>&
                    _map;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_NODE_BUILDER_HH
