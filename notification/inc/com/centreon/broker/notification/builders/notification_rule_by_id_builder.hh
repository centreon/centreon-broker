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

#ifndef CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_ID_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_ID_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/notification_rule_builder.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/objects/notification_rule.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class notification_rule_by_id_builder notification_rule_by_id_builder.hh
 * "com/centreon/broker/notification/builders/notification_rule_by_id_builder.hh"
 *  @brief Notification rule by id builder.
 */
class notification_rule_by_id_builder : public notification_rule_builder {
 public:
  notification_rule_by_id_builder(
      QHash<uint32_t, objects::notification_rule::ptr>& table);

  void add_rule(uint32_t id, objects::notification_rule::ptr con);

 private:
  QHash<uint32_t, objects::notification_rule::ptr>& _table;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_NOTIFICATION_RULE_BY_ID_BUILDER_HH
