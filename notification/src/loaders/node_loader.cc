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

#include "com/centreon/broker/notification/loaders/node_loader.hh"
#include <QSet>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

node_loader::node_loader() {}

/**
 *  Load the nodes from the database.
 *
 *  @param[in] ms       An open connection to the database.
 *  @param[out] output  A node builder object to register the nodes.
 */
void node_loader::load(mysql* ms, node_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!ms || !output)
    return;
  log_v2::notification()->debug(
      "notification: loading nodes from the database");

  // Performance improvement, as we never go back.
  //  query.setForwardOnly(true);

  // Load hosts.
  std::promise<database::mysql_result> promise;
  ms->run_query_and_get_result("SELECT host_id FROM cfg_hosts", &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      uint32_t host_id = res.value_as_u32(0);
      node::ptr n(new node);
      n->set_node_id(node_id(host_id));
      log_v2::notification()->debug(
          "notification: loading host {} from database", host_id);
      output->add_node(n);
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "notification: cannot load hosts from database: " << e.what();
  }

  promise = std::promise<database::mysql_result>();
  // Load services.
  ms->run_query_and_get_result(
      "SELECT hsr.host_host_id, hsr.service_service_id"
      "  FROM cfg_hosts_services_relations AS hsr"
      "  LEFT JOIN cfg_services AS s"
      "    ON hsr.service_service_id=s.service_id",
      &promise);

  try {
    database::mysql_result res(promise.get_future().get());
    while (ms->fetch_row(res)) {
      uint32_t host_id = res.value_as_u32(0);
      uint32_t service_id = res.value_as_u32(1);
      node::ptr n(new node);
      n->set_node_id(node_id(host_id, service_id));
      log_v2::notification()->debug(
          "notification: loading service ({}, {}) from database", host_id,
          service_id);
      output->add_node(n);
    }
  } catch (std::exception const& e) {
    throw exceptions::msg()
        << "notification: cannot load services from database: " << e.what();
  }
}
