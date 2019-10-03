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

#include "com/centreon/broker/notification/loaders/escalation_loader.hh"
#include <QSqlError>
#include <sstream>
#include <vector>
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

escalation_loader::escalation_loader() {}

/**
 *  Load the escalations from the database.
 *
 *  @param[in] db       An open connection to the database.
 * @param[out] output   An escalation builder object to register the
 * escalations.
 */
void escalation_loader::load(QSqlDatabase* db, escalation_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Load the escalations.
  if (!query.exec("SELECT esc_id, esc_name, esc_alias, first_notification,"
                  "last_notification, notification_interval, escalation_period,"
                  "escalation_options1, escalation_options2"
                  " FROM escalation"))
    throw(exceptions::msg()
          << "Notification: cannot select escalation in loader: "
          << query.lastError().text());

  while (query.next()) {
    escalation::ptr esc(new escalation);
    uint32_t id = query.value(0).toUInt();

    esc->set_first_notification(query.value(3).toUInt());
    esc->set_last_notification(query.value(4).toUInt());
    esc->set_notification_interval(query.value(5).toUInt());
    esc->set_escalation_period(query.value(6).toString().toStdString());
    esc->parse_host_escalation_options(query.value(7).toString().toStdString());
    esc->parse_service_escalation_options(
        query.value(7).toString().toStdString());

    output->add_escalation(id, esc);
  }

  // Load relations
  _load_relations(query, *output);
}

void escalation_loader::_load_relations(QSqlQuery& query,
                                        escalation_builder& output) {
  // Load the escalation to node id relations.
  if (!query.exec("SELECT escalation_esc_id, host_host_id"
                  " FROM escalation_host_relation"))
    throw(exceptions::msg()
          << "Notification: cannot select escalation_host_relation in loader: "
          << query.lastError().text());
  while (query.next())
    output.connect_escalation_node_id(query.value(0).toUInt(),
                                      node_id(query.value(1).toUInt()));

  if (!query.exec("SELECT escalation_esc_id, host_host_id, service_service_id"
                  " FROM escalation_service_relation"))
    throw(exceptions::msg()
          << "Notification: cannot select escalation_host_relation in loader: "
          << query.lastError().text());
  while (query.next())
    output.connect_escalation_node_id(
        query.value(0).toUInt(),
        node_id(query.value(1).toUInt(), query.value(2).toUInt()));

  // Load the escalation to contactgroup relations.
  _load_relation(query, output, "contactgroup_cg_id",
                 "escalation_contactgroup_relation",
                 &escalation_builder::connect_escalation_contactgroup);
  // Load the escalation to hostgroup relations.
  _load_relation(query, output, "hostgroup_hg_id",
                 "escalation_hostgroup_relation",
                 &escalation_builder::connect_escalation_hostgroup);
  // Load the escalation to servicegroup relation.
  _load_relation(query, output, "servicegroup_sg_id",
                 "escalation_servicegroup_relation",
                 &escalation_builder::connect_escalation_servicegroup);
}

/**
 *  Load a relation between two ids, one of which is escalation_esc_id, from a
 * table in the database.
 *
 *  @param[in,out] query                A query object linked to the db.
 *  @param[out] output                  The output escalation builder.
 *  @param[in] relation_id_name         The name of the second relation
 * identifier.
 *  @param[in] table                    The table to load the relations from.
 *  @param[in] register_method          The escalation builder method to call to
 * register the escalation.
 */
void escalation_loader::_load_relation(
    QSqlQuery& query,
    escalation_builder& output,
    std::string const& relation_id_name,
    std::string const& table,
    void (escalation_builder::*register_method)(uint32_t, uint32_t)) {
  std::stringstream ss;
  ss << "SELECT escalation_esc_id, " << relation_id_name << " FROM " << table;
  if (!query.exec(ss.str().c_str()))
    throw(exceptions::msg() << "Notification: cannot select " << table
                            << " in loader: " << query.lastError().text());

  while (query.next()) {
    uint32_t id = query.value(0).toUInt();
    uint32_t associated_id = query.value(1).toUInt();

    (output.*register_method)(id, associated_id);
  }
}
