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

#include <utility>
#include <vector>
#include <sstream>
#include <QVariant>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/objects/dependency.hh"
#include "com/centreon/broker/notification/loaders/dependency_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

dependency_loader::dependency_loader() {}

/**
 *  Load the dependencies from the database.
 *
 *  @param[in] db       An open connection to the database.
 * @param[out] output   A dependency builder object to register the dependencies.
 */
void dependency_loader::load(QSqlDatabase* db, dependency_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  // We do not know the type of a dependency until far latter.
  // Cache the options until we know enough to correctly parse them.
  std::vector<std::pair<int, std::string> > dep_execution_failure_options;
  std::vector<std::pair<int, std::string > > dep_notification_failure_options;
  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  if (!query.exec("SELECT dep_id, dep_name, dep_description, inherits_parent,"
                  "execution_failure_criteria, notification_failure_criteria"
                  " FROM dependency"))
    throw (exceptions::msg()
      << "Notification: cannot select dependency in loader: "
      << query.lastError().text());

  while (query.next()) {
    dependency::ptr dep(new dependency);
    unsigned int id = query.value(0).toUInt();
    dep->set_inherits_parent(query.value(3).toBool());
    dep_execution_failure_options.push_back(
          std::make_pair(id, query.value(4).toString().toStdString()));
    dep_notification_failure_options.push_back(
          std::make_pair(id, query.value(5).toString().toStdString()));

    output->add_dependency(id, dep);
  }

  // Get the relations of the dependencies. It will also give us their type.
  _load_relations(query, *output);

  // We now know the types of the dependencies: we can parse the failure options.
  for (std::vector<std::pair<int, std::string> >::const_iterator
       it(dep_execution_failure_options.begin()),
       end(dep_execution_failure_options.end());
       it != end; ++it)
    output->set_execution_failure_options(it->first, it->second);
  for (std::vector<std::pair<int, std::string> >::const_iterator
       it(dep_notification_failure_options.begin()),
       end(dep_notification_failure_options.end());
       it != end; ++it)
    output->set_notification_failure_options(it->first, it->second);
}

/**
 *  Load the relations from the database.
 *
 *  @param[in] query    A query object linked to the db.
 *  @param[out] output  An output dependency builder to load the relations.
 */
void dependency_loader::_load_relations(QSqlQuery& query,
                                        dependency_builder& output) {

  if (!query.exec("SELECT dependency_dep_id, host_host_id"
                  " FROM dependency_hostChild_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select dependency_hostChild_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.dependency_node_id_child_relation(query.value(0).toUInt(),
                                             node_id(query.value(1).toUInt()));

  if (!query.exec("SELECT dependency_dep_id, host_host_id"
                  " FROM dependency_hostParent_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select dependency_hostParent_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.dependency_node_id_parent_relation(query.value(0).toUInt(),
                                              node_id(query.value(1).toUInt()));

  if (!query.exec("SELECT dependency_dep_id, service_service_id, host_host_id"
                  " FROM dependency_serviceChild_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select dependency_serviceChild_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.dependency_node_id_child_relation(query.value(0).toUInt(),
                                             node_id(query.value(2).toUInt(),
                                                     query.value(1).toUInt()));

  if (!query.exec("SELECT dependency_dep_id, service_service_id, host_host_id"
                  " FROM dependency_serviceParent_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select dependency_serviceParent_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.dependency_node_id_parent_relation(query.value(0).toUInt(),
                                              node_id(query.value(2).toUInt(),
                                                      query.value(1).toUInt()));
  _load_relation(query, output,
                 "servicegroup_sg_id",
                 "dependency_servicegroupParent_relation",
                  &dependency_builder::dependency_servicegroup_parent_relation);
  _load_relation(query, output,
                 "servicegroup_sg_id",
                 "dependency_servicegroupChild_relation",
                  &dependency_builder::dependency_servicegroup_child_relation);
  _load_relation(query, output,
                 "hostgroup_hg_id",
                 "dependency_hostgroupParent_relation",
                  &dependency_builder::dependency_hostgroup_parent_relation);
  _load_relation(query, output,
                 "hostgroup_hg_id",
                 "dependency_hostgroupChild_relation",
                  &dependency_builder::dependency_hostgroup_child_relation);
}

/**
 *  Load a relation between two ids, one of which is dependency_dep_id, from a table in the database.
 *
 *  @param[in,out] query                A query object linked to the db.
 *  @param[out] output                  The output dependency builder.
 *  @param[in] relation_id_name         The name of the second relation identifier.
 *  @param[in] table                    The table to load the relations from.
 *  @param[in] register_method          The dependency builder method to call to register the escalation.
 */
void dependency_loader::_load_relation(
       QSqlQuery& query,
       dependency_builder& output,
       std::string const& relation_id_name,
       std::string const& table,
       void (dependency_builder::*register_method)(unsigned int, unsigned int)) {
  std::stringstream ss;
  ss << "SELECT dependency_dep_id, " << relation_id_name << " FROM " << table;
  if (!query.exec(ss.str().c_str()))
    throw (exceptions::msg()
      << "Notification: cannot select " <<  table << " in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();
    unsigned int associated_id = query.value(1).toUInt();

    (output.*register_method)(id, associated_id);
  }
}
