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

#include "com/centreon/broker/notification/builders/composed_notification_rule_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_notification_rule_builder::composed_notification_rule_builder() {}

/**
 *  Add a notification rule to the builder.
 *
 *  @param[in] rule_id  The id of the notification rule.
 *  @param[in] rule     The rule.
 */
void composed_notification_rule_builder::add_rule(
    uint32_t rule_id,
    objects::notification_rule::ptr rule) {
  for (composed_builder<notification_rule_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_rule(rule_id, rule);
}
