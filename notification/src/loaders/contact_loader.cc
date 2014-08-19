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

#include <QVariant>
#include <QSqlError>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/loaders/contact_loader.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;

contact_loader::contact_loader() {}

static void _parse_host_notification_options(std::string const& line,
                                             contact& cont);
static void _parse_service_notification_options(std::string const& line,
                                                contact& cont);

void contact_loader::load(QSqlDatabase* db, contact_builder* output) {
  // If we don't have any db or output, don't do anything.
  if (!db || !output)
    return;

  QSqlQuery query(*db);

  if (!query.exec("SELECT contact_id, timeperiod_tp_id, timeperiod_tp_id2, contact_name, contact_alias, contact_host_notification_options, contact_service_notification_options, contact_email, contact_pager, contact_address1, contact_address2, contact_address3, contact_address4, contact_address5, contact_address6, contact_enable_notifications from contact"))
    throw (exceptions::msg()
      << "Notification: cannot select contact in loader: "
      << query.lastError().text());

  while (query.next()) {
    shared_ptr<contact> cont(new contact);
    unsigned int id = query.value(0).toUInt();
    cont->set_host_notification_period(query.value(1).toString().toStdString());
    cont->set_service_notification_period(query.value(2).toString().toStdString());
    cont->set_name(query.value(3).toString().toStdString());
    cont->set_alias(query.value(4).toString().toStdString());
    _parse_host_notification_options(query.value(5).toString().toStdString(),
                                     *cont);
    _parse_service_notification_options(query.value(6).toString().toStdString(),
                                        *cont);
    cont->set_email(query.value(7).toString().toStdString());
    cont->set_pager(query.value(8).toString().toStdString());
    cont->add_address(query.value(9).toString().toStdString());
    cont->add_address(query.value(10).toString().toStdString());
    cont->add_address(query.value(11).toString().toStdString());
    cont->add_address(query.value(12).toString().toStdString());
    cont->add_address(query.value(13).toString().toStdString());
    cont->add_address(query.value(14).toString().toStdString());
    //cont->set_host_notification_enabled(query.value(15).toInt() ==)

    output->add_contact(id, cont);
  }

  if (!query.exec("SELECT contact_contact_id, contactgroup_cg_id from contactgroup_contact_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select contactgroup_contact_relation in loader: "
      << query.lastError().text());

  while (query.next()) {
    unsigned int contact_id = query.value(0).toUInt();
    unsigned int contactgroup_id = query.value(1).toUInt();

    output->connect_contactgroup_contact(contact_id, contactgroup_id);
  }

}

static void _parse_host_notification_options(std::string const& line,
                                             contact& cont) {
  if (line == "n")
    cont.set_host_notification_options(contact::host_none);
  else {
    std::list<std::string> tokens;
    string::split(line, tokens, ',');

    for (std::list<std::string>::const_iterator it(tokens.begin()),
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


static void _parse_service_notification_options(std::string const& line,
                                                contact& cont) {
  if (line == "n")
    cont.set_service_notification_options(contact::service_none);
  else {
    std::list<std::string> tokens;
    string::split(line, tokens, ',');

    for (std::list<std::string>::const_iterator it(tokens.begin()),
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
