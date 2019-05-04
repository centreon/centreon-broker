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
#include "com/centreon/broker/notification/objects/notification_method.hh"
#include "com/centreon/broker/notification/loaders/notification_method_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

notification_method_loader::notification_method_loader() {}

void notification_method_loader::load(
                                   mysql *ms,
                                   notification_method_builder *output) {
  // If we don't have any db or output, don't do anything.
  if (!ms || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading notification methods from the database";

  // Performance improvement, as we never go back.
  //query.setForwardOnly(true);

  std::promise<database::mysql_result> promise;
  ms->run_query_and_get_result(
        "SELECT method_id, name, command_id, `interval`, status, "
        "       types, start, end "
        "  FROM cfg_notification_methods",
        &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      notification_method::ptr nm(new notification_method);
      nm->set_name(res.value_as_str(1));
      nm->set_command_id(res.value_as_u32(2));
      nm->set_interval(res.value_as_u32(3));
      nm->set_status(res.value_as_str(4));
      nm->set_types(res.value_as_str(5));
      nm->set_start(res.value_as_u32(6));
      nm->set_end(res.value_as_u32(7));
      logging::debug(logging::low)
        << "notification: new method " << res.value_as_u32(0)
        << " ('" << nm->get_name() << "')";
      output->add_notification_method(res.value_as_u32(0), nm);
    }
  }
  catch (std::exception const& e) {
    throw exceptions::msg()
      << "notification: cannot load notification methods from database: "
      << e.what();
  }
}
