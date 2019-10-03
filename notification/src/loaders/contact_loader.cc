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

#include "com/centreon/broker/notification/loaders/contact_loader.hh"
#include <QSet>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/objects/contact.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

contact_loader::contact_loader() {}

/**
 *  Load the contacts from the database.
 *
 *  @param[in] db       An open connection to the database.
 * @param[out] output   A contact builder object to register the contacts.
 */
void contact_loader::load(mysql* ms, contact_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!ms || !output)
    return;

  logging::debug(logging::medium)
      << "notification: loading contacts from the database";

  // Load the contacts.
  std::promise<database::mysql_result> promise;
  ms->run_query_and_get_result(
      "SELECT contact_id, description FROM cfg_contacts", &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      contact::ptr cont(new contact);
      uint32_t id = res.value_as_u32(0);
      cont->set_id(id);
      cont->set_description(res.value_as_str(1));
      output->add_contact(id, cont);
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "notification: cannot load contacts from database: " << e.what();
  }

  // Load the infos of this contact.
  promise = std::promise<database::mysql_result>();
  ms->run_query_and_get_result(
      "SELECT contact_id, info_key, info_value FROM cfg_contacts_infos",
      &promise);

  try {
    database::mysql_result res(promise.get_future().get());

    while (ms->fetch_row(res)) {
      output->add_contact_info(res.value_as_u32(0), res.value_as_str(1),
                               res.value_as_str(2));
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "notification: cannot load contacts infos from database: "
        << e.what();
  }
}
