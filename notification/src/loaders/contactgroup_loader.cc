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
#include "com/centreon/broker/notification/loaders/contactgroup_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

contactgroup_loader::contactgroup_loader() {}

/**
 *  Load the nodegroups from the database.
 *
 *  @param[in] db       An open connection to the database.
 *  @param[out] output  A nodegroup builder object to register the nodegroups.
 */
void contactgroup_loader::load(
                            QSqlDatabase* db, contactgroup_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading contactgroups from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Get contactgroups.
  if (!query.exec("SELECT cg_id, cg_name, cg_alias FROM cfg_contactgroups"))
    throw (exceptions::msg()
           << "notification: cannot load contact groups from database: "
           << query.lastError().text());

  while (query.next()) {
    unsigned int id = query.value(0).toUInt();
    objects::contactgroup::ptr ctg(new objects::contactgroup);
    ctg->set_name(query.value(1).toString().toStdString());
    ctg->set_alias(query.value(2).toString().toStdString());
    output->add_contactgroup(id, ctg);
  }

  // Get contactgroup contact relations.
  if (!query.exec("SELECT contact_contact_id, contactgroup_cg_id"
                  "  FROM cfg_contactgroups_contacts_relations"))
    throw (exceptions::msg()
           << "notification: cannot load memberships of contact groups "
              "from database: "
           << query.lastError().text());

  while (query.next()) {

  }
}
