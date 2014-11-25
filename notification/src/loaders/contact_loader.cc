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
#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/loaders/contact_loader.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

contact_loader::contact_loader() {}

static void _parse_host_notification_options(
              std::string const& line,
              contact& cont);
static void _parse_service_notification_options(
              std::string const& line,
              contact& cont);

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
  if (!query.exec("SELECT contact_id, timeperiod_tp_id, timeperiod_tp_id2,"
                  "       contact_name, contact_alias,"
                  "       contact_host_notification_options,"
                  "       contact_service_notification_options, contact_email,"
                  "       contact_pager, contact_address1, contact_address2,"
                  "       contact_address3, contact_address4, contact_address5,"
                  "       contact_address6, contact_enable_notifications"
                  "  FROM cfg_contacts"))
    throw (exceptions::msg()
           << "notification: cannot load contacts from database: "
           << query.lastError().text());

  while (query.next()) {
    contact::ptr cont(new contact);
    unsigned int id = query.value(0).toUInt();
    cont->set_host_notification_period(
      query.value(1).toString().toStdString());
    cont->set_service_notification_period(
      query.value(2).toString().toStdString());
    cont->set_name(query.value(3).toString().toStdString());
    cont->set_alias(query.value(4).toString().toStdString());
    _parse_host_notification_options(
      query.value(5).toString().toStdString(),
      *cont);
    _parse_service_notification_options(
      query.value(6).toString().toStdString(),
      *cont);
    cont->set_email(query.value(7).toString().toStdString());
    cont->set_pager(query.value(8).toString().toStdString());
    cont->add_address(query.value(9).toString().toStdString());
    cont->add_address(query.value(10).toString().toStdString());
    cont->add_address(query.value(11).toString().toStdString());
    cont->add_address(query.value(12).toString().toStdString());
    cont->add_address(query.value(13).toString().toStdString());
    cont->add_address(query.value(14).toString().toStdString());
    bool is_enabled = (query.value(15).toInt() == 0);
    cont->set_host_notifications_enabled(is_enabled);
    cont->set_service_notifications_enabled(is_enabled);

    output->add_contact(id, cont);
  }

  // Load the custom variables of the contact.
  if (!query.exec("SELECT cp_key, cp_value, cp_contact_id "
                  "  FROM cfg_contacts_params"))
    throw (exceptions::msg()
           << "notification: cannot load contacts parameters from database: "
           << query.lastError().text());

  while (query.next()) {
    output->add_contact_param(
              query.value(2).toUInt(),
              query.value(0).toString().toStdString(),
              query.value(1).toString().toStdString());
  }
}

static void _parse_host_notification_options(
              std::string const& line,
              contact& cont) {
  if (line == "n")
    cont.set_host_notification_options(contact::host_none);
  else {
    std::vector<std::string> tokens;
    string::split(line, tokens, ',');

    for (std::vector<std::string>::const_iterator it(tokens.begin()),
         end(tokens.end()); it != end; ++it) {
      if (*it == "d")
        cont.set_host_notification_option(contact::host_down);
      else if (*it == "u")
        cont.set_host_notification_option(contact::host_unreachable);
      else if (*it == "r")
        cont.set_host_notification_option(contact::host_up);
      else if (*it == "f")
        cont.set_host_notification_option(contact::host_flapping);
      else if (*it == "s")
        cont.set_host_notification_option(contact::host_downtime);
    }
  }
}

/**
 *  Parse the notification options from a string list of options.
 *
 *  @param[in] line   The line to parse
 *  @param[out] cont  The object to fill.
 */
static void _parse_service_notification_options(
              std::string const& line,
              contact& cont) {
  if (line == "n")
    cont.set_service_notification_options(contact::service_none);
  else {
    std::vector<std::string> tokens;
    string::split(line, tokens, ',');

    for (std::vector<std::string>::const_iterator it(tokens.begin()),
         end(tokens.end()); it != end; ++it) {
      if (*it == "w")
        cont.set_service_notification_option(contact::service_warning);
      else if (*it == "u")
        cont.set_service_notification_option(contact::service_unknown);
      else if (*it == "c")
        cont.set_service_notification_option(contact::service_critical);
      else if (*it == "r")
        cont.set_service_notification_option(contact::service_ok);
      else if (*it == "f")
        cont.set_service_notification_option(contact::service_flapping);
      else if (*it == "s")
        cont.set_service_notification_option(contact::service_downtime);
    }
  }
}
