/*
** Copyright 2014 Centreon
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

#include <sstream>
#include <QSet>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/loaders/notification_rule_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

notification_rule_loader::notification_rule_loader() {}

void notification_rule_loader::load(
                                 mysql* ms,
                                 notification_rule_builder *output) {
  // If we don't have any db or output, don't do anything.
  if (!ms || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading notification rules from the database";

  // Performance improvement, as we never go back.
  //query.setForwardOnly(true);

  std::promise<database::mysql_result> promise;
  ms->run_query_and_get_result(
        "SELECT rule_id, method_id, timeperiod_id, contact_id, "
        "       host_id, service_id"
        "  FROM rt_notification_rules",
        &promise,
        "notification: cannot load notification rules from database: ");

  database::mysql_result res(promise.get_future().get());
  while (ms->fetch_row(res)) {
    notification_rule::ptr rule(new notification_rule);
    rule->set_id(res.value_as_u32(0));
    rule->set_method_id(res.value_as_u32(1));
    rule->set_timeperiod_id(res.value_as_u32(2));
    rule->set_contact_id(res.value_as_u32(3));
    rule->set_node_id(node_id(res.value_as_u32(4),
                              res.value_as_u32(5)));
    logging::debug(logging::low)
      << "notification: new rule " << rule->get_id()
      << " affecting node (" << rule->get_node_id().get_host_id()
      << ", " << rule->get_node_id().get_service_id()
      << ") using method " << rule->get_method_id();
    output->add_rule(res.value_as_u32(0), rule);
  }
}
