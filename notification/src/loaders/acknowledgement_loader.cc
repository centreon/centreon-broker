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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/objects/acknowledgement.hh"
#include "com/centreon/broker/notification/loaders/acknowledgement_loader.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

acknowledgement_loader::acknowledgement_loader() {}

/**
 *  Load the acknowledgements from the database.
 *
 *  @param[in] db       An open connection to the database.
 * @param[out] output   An acknowledgement builder object to register the acknowledgements.
 */
void acknowledgement_loader::load(
                               QSqlDatabase* db,
                               acknowledgement_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  logging::debug(logging::medium)
    << "notification: loading acknowledgments from the database";

  QSqlQuery query(*db);

  // Performance improvement, as we never go back.
  query.setForwardOnly(true);

  // Load the acknowledgements for the hosts.
  if (!query.exec("SELECT host_id, acknowledgement_type"
                  "  FROM cfg_hosts WHERE acknowledged = true"))
    throw (exceptions::msg()
      << "notification: cannot select cfg_hosts in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int host_id = query.value(0).toUInt();
    acknowledgement::ptr ack(new acknowledgement);
    ack->set_type(acknowledgement::host);
    ack->set_host_id(host_id);
    ack->set_acknowledgement_type(query.value(1).toInt());

    output->add_ack(node_id(host_id), ack);
  }

  // Loads the acknowledgements for the services.
  if (!query.exec("SELECT host_id, service_id, acknowledgement_type"
                  "  FROM cfg_services WHERE acknowledged = true"))
    throw (exceptions::msg()
      << "notification: cannot select cfg_services in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int host_id = query.value(0).toUInt();
    unsigned int service_id = query.value(1).toUInt();
    acknowledgement::ptr ack(new acknowledgement);
    ack->set_type(acknowledgement::service);
    ack->set_host_id(host_id);
    ack->set_service_id(service_id);
    ack->set_acknowledgement_type(query.value(2).toInt());

    output->add_ack(node_id(host_id, service_id), ack);
  }
}

/**
 *  Update the acknowledgements from a new event.
 *
 *  @param[in] new_ack       The acknowledgement event.
 * @param[out] output        The acknowledgement builder used to register the downtime.
 */
void acknowledgement_loader::new_ack(
                               neb::acknowledgement& new_ack,
                               acknowledgement_builder& output) {
  acknowledgement::ptr ack;
  ack->set_acknowledgement_type(new_ack.acknowledgement_type);
  ack->set_host_id(new_ack.host_id);
  ack->set_service_id(new_ack.service_id);
  ack->set_type(new_ack.service_id == 0 ? acknowledgement::host :
                                          acknowledgement::service);

  output.add_ack(node_id(new_ack.host_id, new_ack.service_id), ack);
}
