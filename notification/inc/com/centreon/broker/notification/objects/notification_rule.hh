/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_NOTIFICATION_NOTIFICATION_RULE_HH
#define CCB_NOTIFICATION_NOTIFICATION_RULE_HH

#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/defines.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace notification {
namespace objects {
/**
 *  @class notification_rule notification_rule.hh
 * "com/centreon/broker/notification/objects/notification_rule.hh"
 *  @brief The notification rule object.
 *
 */
class notification_rule {
 public:
  DECLARE_SHARED_PTR(notification_rule);

  notification_rule();
  notification_rule(notification_rule const& obj);
  notification_rule& operator=(notification_rule const& obj);

  uint32_t get_id();
  void set_id(uint32_t id);

  uint32_t get_method_id() const throw();
  void set_method_id(uint32_t) throw();

  uint32_t get_timeperiod_id() const throw();
  void set_timeperiod_id(uint32_t val) throw();

  uint32_t get_contact_id() const throw();
  void set_contact_id(uint32_t val) throw();

  node_id get_node_id() const throw();
  void set_node_id(node_id val) throw();

 private:
  uint32_t _id;
  uint32_t _method_id;
  uint32_t _timeperiod_id;
  uint32_t _contact_id;
  node_id _node_id;
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_NOTIFICATION_RULE_HH
