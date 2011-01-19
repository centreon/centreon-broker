/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <algorithm>
#include <assert.h>
#include <memory>
#include <QtCore>
#include <QtSql>
#include <sstream>
#include <stdlib.h>
#include "events/events.hh"
#include "exceptions/basic.hh"
#include "interface/db/destination.hh"
#include "interface/db/internal.hh"
#include "logging/logging.hh"
#include "mapping.hh"
#include "nagios/broker.h"

using namespace interface::db;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Processing table.
void (destination::* destination::processing_table[])(events::event const&) = {
  NULL,                                          // UNKNOWN
  &destination::_process_acknowledgement,        // ACKNOWLEDGEMENT
  &destination::_process_comment,                // COMMENT
  &destination::_process_custom_variable,        // CUSTOMVARIABLE
  &destination::_process_custom_variable_status, // CUSTOMVARIABLESTATUS
  &destination::_process_downtime,               // DOWNTIME
  &destination::_process_event_handler,          // EVENTHANDLER
  &destination::_process_flapping_status,        // FLAPPINGSTATUS
  &destination::_process_host,                   // HOST
  &destination::_process_host_check,             // HOSTCHECK
  &destination::_process_host_dependency,        // HOSTDEPENDENCY
  &destination::_process_host_group,             // HOSTGROUP
  &destination::_process_host_group_member,      // HOSTGROUPMEMBER
  &destination::_process_host_parent,            // HOSTPARENT
  &destination::_process_host_status,            // HOSTSTATUS
  &destination::_process_issue,                  // ISSUE
  &destination::_process_issue_parent,           // ISSUEPARENT
  &destination::_process_log,                    // LOG
  &destination::_process_notification,           // NOTIFICATION
  &destination::_process_program,                // PROGRAM
  &destination::_process_program_status,         // PROGRAMSTATUS
  &destination::_process_service,                // SERVICE
  &destination::_process_service_check,          // SERVICECHECK
  &destination::_process_service_dependency,     // SERVICEDEPENDENCY
  &destination::_process_service_group,          // SERVICEGROUP
  &destination::_process_service_group_member,   // SERVICEGROUPMEMBER
  &destination::_process_service_status,         // SERVICESTATUS
  &destination::_process_state                   // STATE
};

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  As destination is not copiable, the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  @param[in] dest Unused.
 */
destination::destination(destination const& dest)
  : interface::destination() {
  (void)dest;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  As destination is not copiable, the assignment operator is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  @param[in] dest Unused.
 *
 *  @return This object.
 */
destination& destination::operator=(destination const& dest) {
  (void)dest;
  assert(false);
  abort();
  return (*this);
}

/**
 *  @brief Clean tables with data associated to the instance.
 *
 *  Rather than delete appropriate entries in tables, they are instead
 *  deactivated using a specific flag.
 *
 *  @param[in] instance_id Instance ID to remove.
 */
void destination::_clean_tables(int instance_id) {
  // Disable hosts.
  {
    std::ostringstream ss;
    ss << "UPDATE " << mapped_type<events::host>::table
       << " SET enabled=0 "
       << " WHERE instance_id=" << instance_id;
    _conn->exec(ss.str().c_str());
  }

  // Remove host groups.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<events::host_group>::table
       << " WHERE instance_id=" << instance_id;
    _conn->exec(ss.str().c_str());
  }

  // Remove service groups.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<events::service_group>::table
       << " WHERE instance_id=" << instance_id;
    _conn->exec(ss.str().c_str());
  }

  // Remove host dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<events::host_dependency>::table
       << " WHERE host_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<events::host>::table
       << "   WHERE instance_id=" << instance_id << ")"
          " OR dependent_host_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<events::host>::table
       << "   WHERE instance_id=" << instance_id << ")";
    _conn->exec(ss.str().c_str());
  }

  // Remove host parents.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<events::host_parent>::table
       << " WHERE child_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<events::host>::table
       << "   WHERE instance_id=" << instance_id << ")"
          " OR parent_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<events::host>::table
       << "   WHERE instance_id=" << instance_id << ")";
    _conn->exec(ss.str().c_str());
  }

  // Remove service dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<events::service_dependency>::table
       << " WHERE service_id IN ("
          "  SELECT services.service_id"
          "   FROM " << mapped_type<events::service>::table << " AS services"
       << "   JOIN " << mapped_type<events::host>::table << " AS hosts"
       << "   ON hosts.host_id=services.service_id WHERE hosts.instance_id="
       << instance_id << ")"
          " OR dependent_service_id IN ("
          "  SELECT services.service_id "
          "   FROM " << mapped_type<events::service>::table << " AS services"
          "   JOIN " << mapped_type<events::host>::table << " AS hosts"
          "   ON hosts.host_id=services.service_id WHERE hosts.instance_id="
       << instance_id << ")";
    _conn->exec(ss.str().c_str());
  }

  return ;
}

/**
 *  Insert an object in the DB using its mapping.
 *
 *  @param[in] t Object to insert.
 */
template <typename T>
bool destination::_insert(T const& t) {
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");
  logging::info << logging::LOW << "executing query: " << query.c_str();

  // Execute query.
  QSqlQuery q(*_conn);
  bool ret(q.prepare(query.c_str()));
  if (ret) {
    q << t;
    ret = q.exec();
  }

  return (ret);
}

/**
 *  Prepare an insert statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 */
template <typename T>
bool destination::_prepare_insert(std::auto_ptr<QSqlQuery>& st) {
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<T>::table);
  query.append(" (");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");
  logging::info << logging::LOW << "preparing statement: "
                << query.c_str();

  // Prepare statement.
  st.reset(new QSqlQuery(*_conn));
  return (st->prepare(query.c_str()));
}

/**
 *  Prepare an update statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 *  @param[in]  id List of fields that form an UNIQUE.
 */
template <typename T>
bool destination::_prepare_update(std::auto_ptr<QSqlQuery>& st,
                                  std::vector<std::string> const& id) {
  // Build query string.
  std::string query;
  query = "UPDATE ";
  query.append(mapped_type<T>::table);
  query.append(" SET ");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it)
    if (std::find(id.begin(), id.end(), it->first) == id.end()) {
      query.append(it->first);
      query.append("=:");
      query.append(it->first);
      query.append(", ");
    }
  query.resize(query.size() - 2);
  query.append(" WHERE ");
  for (std::vector<std::string>::const_iterator
         it = id.begin(),
         end = id.end();
       it != end;
       ++it) {
    query.append(*it);
    query.append("=:");
    query.append(*it);
    query.append(" AND ");
  }
  query.resize(query.size() - 5);
  logging::info << logging::LOW << "preparing statement: "
                << query.c_str();

  // Prepare statement.
  st.reset(new QSqlQuery(*_conn));
  return (st->prepare(query.c_str()));
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 */
void destination::_process_acknowledgement(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM
                << "processing acknowledgement event";

  // Processing.
  events::acknowledgement const& ack(
    *static_cast<events::acknowledgement const*>(&e));
  if (!_insert(ack)) {
    *_acknowledgement_stmt << ack;
    _acknowledgement_stmt->exec();
  }

  return ;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e Uncasted comment.
 */
void destination::_process_comment(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing comment event";

  // Processing.
  events::comment const& c(*static_cast<events::comment const*>(&e));
  if (!_insert(c)) {
    *_comment_stmt << c;
    _comment_stmt->exec();
  }

  return ;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void destination::_process_custom_variable(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing custom variable event";

  // Processing.
  events::custom_variable const& cv(
    *static_cast<events::custom_variable const*>(&e));
  _insert(cv);

  return ;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void destination::_process_custom_variable_status(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing custom variable status event";

  // Processing.
  events::custom_variable_status const& cvs(
    *static_cast<events::custom_variable_status const*>(&e));
  *_custom_variable_status_stmt << cvs;
  _custom_variable_status_stmt->exec();

  return ;
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 */
void destination::_process_downtime(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing downtime event";

  // Processing.
  events::downtime const& d(*static_cast<events::downtime const*>(&e));
  if (!_insert(d)) {
    *_downtime_stmt << d;
    _downtime_stmt->exec();
  }

  return ;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 */
void destination::_process_event_handler(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing event handler event";

  // Processing.
  events::event_handler const& eh(
    *static_cast<events::event_handler const*>(&e));
  if (!_insert(eh)) {
    *_event_handler_stmt << eh;
    _event_handler_stmt->exec();
  }

  return ;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 */
void destination::_process_flapping_status(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing flapping status event";

  // Processing.
  events::flapping_status const& fs(
    *static_cast<events::flapping_status const*>(&e));
  if (!_insert(fs)) {
    *_flapping_status_stmt << fs;
    _flapping_status_stmt->exec();
  }

  return ;
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void destination::_process_host(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host event";

  // Processing.
  events::host const& h(*static_cast<events::host const*>(&e));
  *_host_stmt << h;
  _host_stmt->exec();
  int matched = _host_stmt->numRowsAffected();
  if (!matched || (-1 == matched))
    _insert(h);

  return ;
}

/**
 *  Process an host check event.
 *
 *  @param[in] e Uncasted host check.
 */
void destination::_process_host_check(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host check event";

  // Processing.
  events::host_check const& hc(
    *static_cast<events::host_check const*>(&e));
  *_host_check_stmt << hc;
  _host_check_stmt->exec();

  return ;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 */
void destination::_process_host_dependency(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host dependency event";

  // Processing.
  events::host_dependency const& hd(
    *static_cast<events::host_dependency const*>(&e));
  _insert(hd);

  return ;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void destination::_process_host_group(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host group event";

  // Processing.
  events::host_group const& hg(
    *static_cast<events::host_group const*>(&e));
  _insert(hg);

  return ;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void destination::_process_host_group_member(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host group member event";

  // Fetch proper structure.
  events::host_group_member const& hgm(
    *static_cast<events::host_group_member const*>(&e));

  // Fetch host group ID.
  std::ostringstream ss;
  ss << "SELECT hostgroup_id FROM "
     << mapped_type<events::host_group>::table
     << " WHERE instance_id=" << hgm.instance_id
     << " AND name=\"" << hgm.group << "\"";
  QSqlQuery q(*_conn);
  logging::info << logging::LOW << "executing query: "
                << ss.str().c_str();
  if (q.exec(ss.str().c_str()) && q.next()) {
    // Fetch hostgroup ID.
    int hostgroup_id(q.value(0).toInt());
    logging::debug << logging::MEDIUM
                   << "fetch hostgroup of id " << hostgroup_id;

    // Insert hostgroup membership.
    std::ostringstream oss;
    oss << "INSERT INTO "
        << mapped_type<events::host_group_member>::table
        << " (host_id, hostgroup_id) VALUES("
        << hgm.host_id << ", "
        << hostgroup_id << ")";
    logging::info << logging::LOW << "executing query: "
                  << ss.str().c_str();
    _conn->exec(ss.str().c_str());
  }
  else
    logging::info << logging::HIGH
                  << "discarding membership between host "
                  << hgm.host_id << " and hostgroup ("
                  << hgm.instance_id << ", " << hgm.group.c_str()
                  << ")";

  return ;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 */
void destination::_process_host_parent(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host parent event";

  // Processing.
  events::host_parent const& hp(
    *static_cast<events::host_parent const*>(&e));
  _insert(hp);

  return ;
}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 */
void destination::_process_host_status(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing host status event";

  // Processing.
  events::host_status const& hs(
    *static_cast<events::host_status const*>(&e));
  *_host_status_stmt << hs;
  _host_status_stmt->exec();

  return ;
}

/**
 *  Process an issue event.
 *
 *  @param[in] e Uncasted issue.
 */
void destination::_process_issue(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing issue event";

  // Processing.
  events::issue const& i(*static_cast<events::issue const*>(&e));
  if (!_insert(i)) {
    *_issue_stmt << i;
    _issue_stmt->exec();
  }

  return ;
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] e Uncasted issue parent.
 */
void destination::_process_issue_parent(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing issue parent event";

  // Fetch proper structure.
  events::issue_parent const& ip(
    *static_cast<events::issue_parent const*>(&e));
  int child_id;
  int parent_id;

  // Get child ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << mapped_type<events::issue>::table << " WHERE host_id="
          << ip.child_host_id << " AND service_id="
          << ip.child_service_id << " AND start_time="
          << ip.child_start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    QSqlQuery q(*_conn);
    if (q.exec(query.str().c_str()) && q.next()) {
      child_id = q.value(0).toInt();
      logging::debug << logging::LOW << "child issue ID: " << child_id;
    }
    else
      throw (exceptions::basic() << "could not fetch child issue ID ("
                                 << "host=" << ip.child_host_id
                                 << ", service=" << ip.child_service_id
                                 << ", start=" << ip.child_start_time
                                 << ")");
  }

  // Get parent ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << mapped_type<events::issue>::table << " WHERE host_id="
          << ip.parent_host_id << " AND service_id="
          << ip.parent_service_id << " AND start_time="
          << ip.parent_start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    QSqlQuery q(*_conn);
    if (q.exec(query.str().c_str()) && q.next()) {
      parent_id = q.value(0).toInt();
      logging::debug << logging::LOW << "parent issue ID: " << parent_id;
    }
    else
      throw (exceptions::basic() << "could not fetch parent issue ID ("
                                 << "host=" << ip.parent_host_id
                                 << ", service=" << ip.parent_service_id
                                 << ", start=" << ip.parent_start_time
                                 << ")");
  }

  // End of parenting.
  if (ip.end_time) {
    std::ostringstream query;
    query << "UPDATE "
          << mapped_type<events::issue_parent>::table
          << " SET end_time="
          << ip.end_time << " WHERE child_id="
          << child_id << " AND parent_id="
          << parent_id << " AND start_time="
          << ip.start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    _conn->exec(query.str().c_str());
  }
  // New parenting.
  else {
    std::ostringstream query;
    query << "INSERT INTO "
          << mapped_type<events::issue_parent>::table
          << " (child_id, parent_id, start_time) VALUES("
          << child_id << ", "
          << parent_id << ", "
          << ip.start_time << ")";
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    _conn->exec(query.str().c_str());
  }

  return ;
}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 */
void destination::_process_log(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing log event";

  // Fetch proper structure.
  events::log_entry const& le(
    *static_cast<events::log_entry const*>(&e));

  // Fetch issue ID (if any).
  int issue;
  if (le.issue_start_time) {
    std::ostringstream ss;
    ss << "SELECT issue_id FROM "
       << mapped_type<events::issue>::table
       << " WHERE host_id=" << le.host_id
       << " AND service_id=" << le.service_id
       << " AND start_time=" << le.issue_start_time;
    logging::info << logging::LOW << "executing query: "
                  << ss.str().c_str();
    QSqlQuery q(*_conn);
    if (q.exec(ss.str().c_str()) && q.next())
      issue = q.value(0).toInt();
    else
      issue = 0;
  }
  else
    issue = 0;

  // Build insertion query.
  char const* field("issue_id");
  std::string query;
  query = "INSERT INTO ";
  query.append(mapped_type<events::log_entry>::table);
  query.append("(");
  for (std::list<std::pair<std::string, getter_setter<events::log_entry> > >::const_iterator
         it = db_mapped_type<events::log_entry>::list.begin(),
         end = db_mapped_type<events::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.append(field);
  query.append(") VALUES(");
  for (std::list<std::pair<std::string, getter_setter<events::log_entry> > >::const_iterator
         it = db_mapped_type<events::log_entry>::list.begin(),
         end = db_mapped_type<events::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.append(":");
  query.append(field);
  query.append(")");

  // Execute query.
  logging::info << logging::LOW << "executing query: " << query.c_str();
  QSqlQuery q(*_conn);
  q.prepare(query.c_str());
  q << le;
  q.bindValue(field, issue);
  q.exec();

  return ;
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void destination::_process_notification(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing notification event";

  // Processing.
  events::notification const& n(
    *static_cast<events::notification const*>(&e));
  if (!_insert(n)) {
    *_notification_stmt << n;
    _notification_stmt->exec();
  }

  return ;
}

/**
 *  Process a program event.
 *
 *  @param[in] e Uncasted program.
 */
void destination::_process_program(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing program event";

  // Clean tables.
  events::program const& p(*static_cast<events::program const*>(&e));
  _clean_tables(p.instance_id);

  // Program start.
  if (!p.program_end) {
    if (!_insert(p)) {
      *_program_stmt << p;
      _program_stmt->exec();
    }
  }
  // Program end.
  else {
    *_program_stmt << p;
    _program_stmt->exec();
  }

  return ;
}

/**
 *  Process a program status event.
 *
 *  @param[in] e Uncasted program status.
 */
void destination::_process_program_status(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing program status event";

  // Processing.
  events::program_status const& ps(
    *static_cast<events::program_status const*>(&e));
  *_program_status_stmt << ps;
  _program_status_stmt->exec();

  return ;
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void destination::_process_service(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing service event";

  // Processing.
  events::service const& s(*static_cast<events::service const*>(&e));
  *_service_stmt << s;
  _service_stmt->exec();
  int matched = _service_stmt->numRowsAffected();
  if (!matched || (-1 == matched)) {
    *_service_insert_stmt << s;
    _service_insert_stmt->exec();
  }

  return ;
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 */
void destination::_process_service_check(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing service check event";

  // Processing.
  events::service_check const& sc(
    *static_cast<events::service_check const*>(&e));
  *_service_check_stmt << sc;
  _service_check_stmt->exec();

  return ;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 */
void destination::_process_service_dependency(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing ServiceDependency event";

  // Processing.
  events::service_dependency const& sd(
    *static_cast<events::service_dependency const*>(&e));
  _insert(sd);

  return ;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void destination::_process_service_group(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing service group event";

  // Processing.
  events::service_group const& sg(
    *static_cast<events::service_group const*>(&e));
  _insert(sg);

  return ;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void destination::_process_service_group_member(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing service group member event";

  // Fetch proper structure.
  events::service_group_member const& sgm(
    *static_cast<events::service_group_member const*>(&e));

  // Fetch service group ID.
  std::ostringstream ss;
  ss << "SELECT servicegroup_id FROM "
     << mapped_type<events::service_group>::table
     << " WHERE instance_id=" << sgm.instance_id
     << " AND name=\"" << sgm.group << "\"";
  QSqlQuery q(*_conn);
  logging::info << logging::LOW << "executing query: "
                << ss.str().c_str();
  if (q.exec(ss.str().c_str()) && q.next()) {
    // Fetch servicegroup ID.
    int servicegroup_id(q.value(0).toInt());
    logging::debug << logging::MEDIUM
                   << "fetch servicegroup of id " << servicegroup_id;

    // Insert servicegroup membership.
    std::ostringstream oss;
    ss << "INSERT INTO "
       << mapped_type<events::service_group_member>::table
       << " (host_id, service_id, servicegroup_id) VALUES("
       << sgm.host_id << ", "
       << sgm.service_id << ", "
       << servicegroup_id << ")";
    logging::info << logging::LOW << "executing query: "
                  << ss.str().c_str();
    _conn->exec(ss.str().c_str());
  }
  else
    logging::info << logging::HIGH
                  << "discarding membership between service ("
                  << sgm.host_id << ", " << sgm.service_id
                  << ") and servicegroup (" << sgm.instance_id
                  << ", " << sgm.group.c_str() << ")";

  return ;
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 */
void destination::_process_service_status(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing service status event";

  // Processing.
  events::service_status const& ss(
    *static_cast<events::service_status const*>(&e));
  *_service_status_stmt << ss;
  _service_status_stmt->exec();

  return ;
}

/**
 *  Process a state event.
 *
 *  @param[in] e Uncasted state.
 */
void destination::_process_state(events::event const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "processing state event";

  // Processing.
  events::state const& s(*static_cast<events::state const*>(&e));
  if (s.end_time) {
    *_state_stmt << s;
    _state_stmt->exec();
  }
  else
    _insert(s);

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Destination default constructor.
 *
 *  Initialize the destination with default parameters.
 */
destination::destination() {
  assert((sizeof(processing_table) / sizeof(*processing_table))
         == events::event::EVENT_TYPES_NB);
}

/**
 *  @brief Destination destructor.
 *
 *  Release all previously allocated ressources.
 */
destination::~destination() {
  this->close();
}

/**
 *  Close the event destination.
 */
void destination::close() {
  _acknowledgement_stmt.reset();
  _comment_stmt.reset();
  _custom_variable_status_stmt.reset();
  _downtime_stmt.reset();
  _event_handler_stmt.reset();
  _flapping_status_stmt.reset();
  _host_stmt.reset();
  _host_check_stmt.reset();
  _host_status_stmt.reset();
  _issue_stmt.reset();
  _notification_stmt.reset();
  _program_stmt.reset();
  _program_status_stmt.reset();
  _service_stmt.reset();
  _service_insert_stmt.reset();
  _service_check_stmt.reset();
  _service_status_stmt.reset();
  _state_stmt.reset();
  _conn.reset();
  return ;
}

/**
 *  @brief Process an event.
 *
 *  When an event is poped from the list, it will be processed by this
 *  method. We will determine the true event type and process it
 *  accordingly.
 *
 *  @param[in] e Event that should be stored in the database.
 */
void destination::event(events::event* e) {
  try {
    (this->*processing_table[e->get_type()])(*e);
  }
  catch (...) {
    // Event self deregistration.
    e->remove_reader();

    // Rethrow the exception
    throw ;
  }

  // Event self deregistration.
  e->remove_reader();

  return ;
}

/**
 *  @brief Connect the database destination.
 *
 *  Connect to the specified database using the specified credentials.
 *
 *  @param[in] db_type Database type.
 *  @param[in] host    DB server.
 *  @param[in] db      Database
 *  @param[in] user    User name to use for authentication.
 *  @param[in] pass    Password to use for authentication.
 */
void destination::connect(destination::DB db_type,
                          std::string const& db,
                          std::string const& host,
                          std::string const& user,
                          std::string const& pass) {
  // Set DB type.
  switch (db_type) {
#ifdef USE_MYSQL
   case MYSQL:
    _conn.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL")));
    _conn->setConnectOptions("CLIENT_FOUND_ROWS");
    break ;
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
   case ORACLE:
    _conn.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QOCI")));
    break ;
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
   case POSTGRESQL:
    _conn.reset(new QSqlDatabase(QSqlDatabase::addDatabase("QPSQL")));
    break ;
#endif /* USE_POSTGRESQL */

   default:
    throw exceptions::basic() << "unsupported DBMS requested";
  }

  // Set common parameters.
  _conn->setDatabaseName(db.c_str());
  _conn->setHostName(host.c_str());
  _conn->setUserName(user.c_str());
  _conn->setPassword(pass.c_str());

  // Open database.
  if (!_conn->open())
    throw (exceptions::basic() << "could not open db: "
                               << _conn->lastError().text().toStdString().c_str());

  // Prepare service query.
  _prepare_insert<events::service>(_service_insert_stmt);

  // Prepare update queries.
  std::vector<std::string> id;

  id.clear();
  id.push_back("author_name");
  id.push_back("entry_time");
  id.push_back("host_name");
  id.push_back("instance_name");
  id.push_back("service_description");
  _prepare_update<events::acknowledgement>(_acknowledgement_stmt, id);

  id.clear();
  id.push_back("entry_time");
  id.push_back("instance_name");
  id.push_back("internal_id");
  _prepare_update<events::comment>(_comment_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("name");
  id.push_back("service_id");
  _prepare_update<events::custom_variable_status>(_custom_variable_status_stmt,
    id);

  id.clear();
  id.push_back("entry_time");
  id.push_back("instance_name");
  id.push_back("internal_id");
  _prepare_update<events::downtime>(_downtime_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<events::event_handler>(_event_handler_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("event_time");
  _prepare_update<events::flapping_status>(_flapping_status_stmt, id);

  id.clear();
  id.push_back("host_id");
  _prepare_update<events::host>(_host_stmt, id);

  id.clear();
  id.push_back("host_id");
  _prepare_update<events::host_check>(_host_check_stmt, id);

  id.clear();
  id.push_back("host_id");
  _prepare_update<events::host_status>(_host_status_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<events::issue>(_issue_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<events::notification>(_notification_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<events::state>(_state_stmt, id);

  id.clear();
  id.push_back("instance_id");
  _prepare_update<events::program>(_program_stmt, id);

  id.clear();
  id.push_back("instance_id");
  _prepare_update<events::program_status>(_program_status_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  _prepare_update<events::service>(_service_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  _prepare_update<events::service_check>(_service_check_stmt, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  _prepare_update<events::service_status>(_service_status_stmt, id);

  return ;
}
