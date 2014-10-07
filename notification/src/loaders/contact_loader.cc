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
#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/loaders/contact_loader.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

contact_loader::contact_loader() {}

static void _parse_host_notification_options(std::string const& line,
                                             contact& cont);
static void _parse_service_notification_options(std::string const& line,
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

  QSqlQuery query(*db);

  // Load the contacts.
  if (!query.exec("SELECT contact_id, timeperiod_tp_id, timeperiod_tp_id2,"
                  "contact_name, contact_alias,"
                  "contact_host_notification_options,"
                  "contact_service_notification_options, contact_email,"
                  "contact_pager, contact_address1, contact_address2,"
                  "contact_address3, contact_address4, contact_address5,"
                  "contact_address6, contact_enable_notifications"
                  " FROM contact"))
    throw (exceptions::msg()
      << "Notification: cannot select contact in loader: "
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
    _parse_host_notification_options(query.value(5).toString().toStdString(),
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

  // Load all the relations of a contact.
  _load_relations(query, *output);

  // Load the custom variables of the contact.
  if (!query.exec("SELECT cp_key, cp_value, cp_contact_id FROM contact_param"))
    throw (exceptions::msg()
      << "Notification: cannot select contact_param in loader: "
      << query.lastError().text());

  while (query.next()) {
    output->add_contact_param(query.value(2).toUInt(),
                              query.value(0).toString().toStdString(),
                              query.value(1).toString().toStdString());
  }
}

/**
 *  Load the relations from the database.
 *
 *  @param[in] query    A query object linked to the db.
 *  @param[out] output  An output contact builder to load the relations.
 */
void contact_loader::_load_relations(QSqlQuery& query,
                                     contact_builder& output) {
  _load_relation(query,
                 output,
                 "contact_contact_id",
                 "contactgroup_cg_id",
                 "contactgroup_contact_relation",
                 &contact_builder::connect_contact_contactgroup);

  // Load contact to hostcommands relations.
  if (!query.exec("select a.contact_contact_id, b.command_name"
                  " FROM contact_hostcommands_relation as a"
                  " LEFT JOIN command as b"
                  " ON a.command_command_id = b.command_id;"))
    throw (exceptions::msg()
      << "Notification: cannot select contact_hostcommands_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.connect_contact_hostcommand(
      query.value(0).toUInt(),
      query.value(1).toString().toStdString());

  // Load contact to servicecommands relations.
  if (!query.exec("select a.contact_contact_id, b.command_name"
                  " FROM contact_servicecommands_relation as a"
                  " LEFT JOIN command as b"
                  " ON a.command_command_id = b.command_id;"))
    throw (exceptions::msg()
      << "Notification: cannot select contact_servicecommands_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.connect_contact_servicecommand(
      query.value(0).toUInt(),
      query.value(1).toString().toStdString());

  // Load contact to node relations.
  if (!query.exec("SELECT contact_id, host_host_id"
                  " FROM contact_host_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select contact_host_relation in loader: "
      << query.lastError().text());
  while (query.next())
    output.connect_contact_node_id(query.value(1).toUInt(),
                                   node_id(query.value(0).toUInt()));

  // We cache the services connected to a service/host pair to prevent the
  // twice insertion of services later.
  QSet<unsigned int> service_ids;

  if (!query.exec("SELECT a.host_host_id, a.service_service_id, b.contact_id"
                  " FROM host_service_relation AS a"
                  " LEFT JOIN contact_service_relation AS b"
                  " ON a.service_service_id = b.service_service_id"))
    throw (exceptions::msg()
      << "Notification: cannot select contact_service_relation in loader: "
      << query.lastError().text());
  while (query.next()) {
    service_ids.insert(query.value(1).toUInt());
    output.connect_contact_node_id(
      query.value(2).toUInt(),
      node_id(query.value(0).toUInt(), query.value(1).toUInt()));
  }

  if (!query.exec("SELECT contact_id, service_service_id"
                  " FROM contact_service_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select contact_service_relation in loader: "
      << query.lastError().text());

  while (query.next())
    if(!service_ids.contains(query.value(1).toUInt()))
      output.connect_contact_node_id(query.value(0).toUInt(),
                                     node_id(0, query.value(1).toUInt()));

  // Load contactgroups to node relations.
  if (!query.exec("SELECT host_host_id, contactgroup_cg_id"
                  " FROM contactgroup_host_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select contactgroup_host_relation in loader: "
      << query.lastError().text());
  while (query.next())
    if(!service_ids.contains(query.value(1).toUInt()))
      output.connect_contactgroup_node_id(query.value(1).toUInt(),
                                          node_id(query.value(0).toUInt()));

  service_ids.clear();
  if (!query.exec("SELECT a.host_host_id, a.service_service_id, b.contactgroup_cg_id"
                  " FROM servicegroup_relation AS a"
                  " LEFT JOIN contactgroup_service_relation AS b"
                  " ON a.service_service_id = b.service_service_id"))
    throw (exceptions::msg()
      << "Notification: cannot select contactgroup_service_relation in loader: "
      << query.lastError().text());
  while (query.next()) {
    service_ids.insert(query.value(1).toUInt());
    output.connect_contactgroup_node_id(
      query.value(2).toUInt(),
      node_id(query.value(0).toUInt(), query.value(1).toUInt()));
  }

  if (!query.exec("SELECT contactgroup_id, service_service_id"
                  " FROM contactgroup_service_relation"))
    throw (exceptions::msg()
      << "Notification: cannot select contactgroup_service_relation in loader: "
      << query.lastError().text());

  while (query.next())
    if(!service_ids.contains(query.value(1).toUInt()))
      output.connect_contactgroup_node_id(query.value(0).toUInt(),
                                          node_id(0, query.value(1).toUInt()));

  // Load contactgroups to hostgroups relations.
  _load_relation(query,
                 output,
                 "contactgroup_cg_id",
                 "hostgroup_hg_id",
                 "contactgroup_hostgroup_relation",
                 &contact_builder::connect_contactgroup_hostgroup);

  // Load contactgroups to servicegroups relations.
  _load_relation(query,
                 output,
                 "contactgroup_cg_id",
                 "servicegroup_sg_id",
                 "contactgroup_servicegroup_relation",
                 &contact_builder::connect_contactgroup_servicegroup);
}

/**
 *  Load a relation between two ids from a table in the database.
 *
 *  @param[in,out] query                A query object linked to the db.
 *  @param[out] output                  The output contact builder.
 *  @param[in] first_relation_id_name   The name of the first relation identifier.
 *  @param[in] second_relation_id_name  The name of the second relation identifier.
 *  @param[in] table                    The table to load the relations from.
 *  @param[in] register_method          The contact builder method to call to register the relation.
 */
void contact_loader::_load_relation(QSqlQuery& query,
                                    contact_builder& output,
                                    std::string const& first_relation_id_name,
                                    std::string const& second_relation_id_name,
                                    std::string const& table,
                                    void (contact_builder::*register_method)
                                    (unsigned int, unsigned int)) {
  std::stringstream ss;
  ss << "SELECT " << first_relation_id_name << ", "
     << second_relation_id_name << " FROM " << table;
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

static void _parse_host_notification_options(std::string const& line,
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
static void _parse_service_notification_options(std::string const& line,
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
