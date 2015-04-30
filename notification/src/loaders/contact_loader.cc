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
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/loaders/contact_loader.hh"

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
void contact_loader::load(QSqlDatabase* db, contact_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading contacts from the database";

  QSqlQuery query(*db);

  // Load the contacts.
  if (!query.exec("SELECT contact_id, description"
                  "  FROM cfg_contacts"))
    throw (exceptions::msg()
           << "notification: cannot load contacts from database: "
           << query.lastError().text());

  while (query.next()) {
    contact::ptr cont(new contact);
    unsigned int id = query.value(0).toUInt();
    cont->set_id(id);
    cont->set_description(query.value(1).toString().toStdString());
    output->add_contact(id, cont);
  }

  // Load the infos of this contact.
  if (!query.exec("SELECT contact_id, info_key, info_value "
                  "  FROM cfg_contacts_infos"))
    throw (exceptions::msg()
           << "notification: cannot load contacts infos from database: "
           << query.lastError().text());

  while (query.next()) {
    output->add_contact_info(
              query.value(0).toUInt(),
              query.value(1).toString().toStdString(),
              query.value(2).toString().toStdString());
  }
}
