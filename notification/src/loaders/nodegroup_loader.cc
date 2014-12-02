/*
** Copyright 2014 Merethis
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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/loaders/nodegroup_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

nodegroup_loader::nodegroup_loader() {}

/**
 *  Load the nodegroups from the database.
 *
 *  @param[in] db       An open connection to the database.
 *  @param[out] output  A nodegroup builder object to register the nodegroups.
 */
void nodegroup_loader::load(QSqlDatabase* db, nodegroup_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading nodegroups from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Get hostgroups.
  if (!query.exec("SELECT hg_id, hg_name, hg_alias FROM cfg_hostgroups"))
    throw (exceptions::msg()
           << "notification: cannot load host groups from database: "
           << query.lastError().text());

  while (query.next()) {
    objects::node_id id(query.value(0).toUInt());
    objects::nodegroup::ptr ndg(new objects::nodegroup);
    ndg->set_name(query.value(1).toString().toStdString());
    ndg->set_alias(query.value(2).toString().toStdString());
    output->add_nodegroup(id, ndg);
  }

  // Get servicegroups
  if (!query.exec("SELECT sg_id, sg_name, sg_alias FROM cfg_servicegroups"))
    throw (exceptions::msg()
           << "notification: cannot load service groups from database: "
           << query.lastError().text());

  while (query.next()) {
    objects::node_id id(0, query.value(0).toUInt());
    objects::nodegroup::ptr ndg(new objects::nodegroup);
    ndg->set_name(query.value(1).toString().toStdString());
    ndg->set_alias(query.value(2).toString().toStdString());
    output->add_nodegroup(id, ndg);
  }
}
