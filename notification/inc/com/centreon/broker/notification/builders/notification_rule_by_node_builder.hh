/*
** Copyright 2011-2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_NODE_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_NODE_BUILDER_HH

#include <QHash>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/notification_rule_builder.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class notification_method_by_node_builder
 * notification_method_by_node_builder.hh
 * "com/centreon/broker/notification/builders/notification_method_by_node_builder.hh"
 */
class notification_rule_by_node_builder : public notification_rule_builder {
 public:
  notification_rule_by_node_builder(
      QMultiHash<objects::node_id, objects::notification_rule::ptr>& map);

  virtual void add_rule(uint32_t rule_id,
                        objects::notification_rule::ptr rule);

 private:
  notification_rule_by_node_builder();

  QMultiHash<objects::node_id, objects::notification_rule::ptr>& _map;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_NODE_BUILDER_HH
