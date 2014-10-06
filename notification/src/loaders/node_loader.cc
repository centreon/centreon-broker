/*
** Copyright 2011-2013 Merethis
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

#include <sstream>
#include <QVariant>
#include <QSet>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/loaders/node_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

node_loader::node_loader() {

}

/**
 *  Load the nodes from the database.
 *
 *  @param[in] db       An open connection to the database.
 * @param[out] output   A node builder object to register the nodes.
 */
void node_loader::load(QSqlDatabase* db, node_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Instead of doing crosses select from host_id and service_id,
  // we only do three selects and internally do the connexions. It's faster.

  if (!query.exec("SELECT host_id FROM host"))
    throw (exceptions::msg()
      << "Notification: cannot select host in loader: "
      << query.lastError().text());

  while (query.next()) {
    node::ptr n(new node);
    n->set_node_id(node_id(query.value(0).toUInt()));

    output->add_node(n);
  }

  QSet<unsigned int> service_cache;

  if (!query.exec("SELECT host_host_id, service_service_id"
                  " FROM host_service_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select host_service_relation in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int service_id = query.value(1).toUInt();
    node::ptr n(new node);
    n->set_node_id(node_id(query.value(0).toUInt(), service_id));
    service_cache.insert(service_id);

    output->add_node(n);
  }

  if (!query.exec("SELECT service_id FROM service"))
    throw (exceptions::msg()
      << "Notification: cannot select host in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();
    if (!service_cache.contains(id))
    {
      node::ptr n(new node);
      n->set_node_id(node_id(0, id));

      output->add_node(n);
    }
  }

  // Load host groups
  if (!query.exec("SELECT host_host_id, hostgroup_hg_id"
                  " FROM hostgroup_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select hostgroup_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output->connect_node_hostgroup(node_id(query.value(0).toUInt()),
                                   query.value(1).toUInt());

  // Load service groups
  if (!query.exec("SELECT service_service_id, servicegroup_sg_id, "
                  "host_host_id, hostgroup_hg_id"
                  " FROM servicegroup_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select servicegroup_relation in loader: "
      << query.lastError().text());
  while (query.next()) {
    output->connect_node_servicegroup(node_id(query.value(2).toUInt(),
                                              query.value(0).toUInt()),
                                      query.value(1).toUInt());
    output->connect_hostgroup_servicegroup(query.value(3).toUInt(),
                                           query.value(1).toUInt());
  }

  // Load host groups parent relations.
  if (!query.exec("SELECT hg_child_id, hg_parent_id"
                  " FROM hostgroup_hg_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select hostgroup_hg_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output->connect_hostgroup_parent_hostgroup(query.value(0).toUInt(),
                                               query.value(1).toUInt());
}
