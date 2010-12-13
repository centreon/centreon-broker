/*
**  Copyright 2009-2011 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <algorithm>                  // for find
#include <assert.h>
#include <soci.h>
#ifdef USE_MYSQL
# include <soci-mysql.h>
#endif /* USE_MYSQL */
#ifdef USE_ORACLE
# include <soci-oracle.h>
#endif /* USE_ORACLE */
#ifdef USE_POSTGRESQL
# include <soci-postgresql.h>
#endif /* USE_POSTGRESQL */
#include <sstream>
#include <stdlib.h>                   // for abort
#include "events/events.h"
#include "exception.h"
#include "interface/db/destination.hh"
#include "interface/db/internal.h"
#include "logging/logging.hh"
#include "mapping.h"
#include "nagios/broker.h"

using namespace Interface::DB;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Processing table.
void (destination::* destination::processing_table[])(Events::Event const&) = {
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
 *  @brief Destination copy constructor.
 *
 *  As destination is not copiable, the copy constructor is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  @param[in] dest Unused.
 */
destination::destination(destination const& dest)
  : Interface::Destination() {
  (void)dest;
  assert(false);
  abort();
}

/**
 *  @brief Overload of the assignment operator.
 *
 *  As destination is not copiable, the assignment operator is declared
 *  private. Any attempt to use it will result in a call to abort().
 *
 *  @param[in] dest Unused.
 *
 *  @return *this
 */
destination& destination::operator=(destination const& dest) {
  (void)dest;
  assert(false);
  abort();
  return (*this);
}

/**
 *  Clean tables with data associated to the instance.
 *
 *  @param[in] instance_id Instance ID to remove.
 */
void destination::_clean_tables(int instance_id) {
  // Disable hosts.
  *_conn << "UPDATE " << MappedType<Events::Host>::table
         << " SET enabled=0 "
         << " WHERE instance_id=" << instance_id;

  // Remove groups.
  *_conn << "DELETE FROM " << MappedType<Events::HostGroup>::table
         << " WHERE instance_id=" << instance_id;
  *_conn << "DELETE FROM " << MappedType<Events::ServiceGroup>::table
         << " WHERE instance_id=" << instance_id;

  // Remove host dependencies.
  *_conn << "DELETE FROM " << MappedType<Events::HostDependency>::table
         << " WHERE host_id IN ("
            "  SELECT host_id"
            "   FROM " << MappedType<Events::Host>::table
         << "   WHERE instance_id=" << instance_id << ")"
            " OR dependent_host_id IN ("
            "  SELECT host_id"
            "   FROM " << MappedType<Events::Host>::table
         << "   WHERE instance_id=" << instance_id << ")";

  // Remove host parents.
  *_conn << "DELETE FROM " << MappedType<Events::HostParent>::table
         << " WHERE child_id IN ("
            "  SELECT host_id"
            "   FROM " << MappedType<Events::Host>::table
         << "   WHERE instance_id=" << instance_id << ")"
            " OR parent_id IN ("
            "  SELECT host_id"
            "   FROM " << MappedType<Events::Host>::table
         << "   WHERE instance_id=" << instance_id << ")";

  // Remove service dependencies.
  *_conn << "DELETE FROM " << MappedType<Events::ServiceDependency>::table
         << " WHERE service_id IN ("
            "  SELECT services.service_id"
            "   FROM " << MappedType<Events::Service>::table << " AS services"
         << "   JOIN " << MappedType<Events::Host>::table << " AS hosts"
         << "   ON hosts.host_id=services.service_id WHERE hosts.instance_id="
         << instance_id << ")"
            " OR dependent_service_id IN ("
            "  SELECT services.service_id "
            "   FROM " << MappedType<Events::Service>::table << " AS services"
            "   JOIN " << MappedType<Events::Host>::table << " AS hosts"
            "   ON hosts.host_id=services.service_id WHERE hosts.instance_id="
         << instance_id << ")";

  return ;
}

/**
 *  Insert an object in the DB using its mapping.
 *
 *  @param[in] t Object to insert.
 */
template <typename T>
void destination::_insert(T const& t)
{
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(MappedType<T>::table);
  query.append(" (");
  for (typename std::list<std::pair<std::string,
                                    GetterSetter<T> > >::const_iterator
         it = DBMappedType<T>::list.begin(),
         end = DBMappedType<T>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::list<std::pair<std::string,
                                    GetterSetter<T> > >::const_iterator
         it = DBMappedType<T>::list.begin(),
         end = DBMappedType<T>::list.end();
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
  *_conn << query, soci::use(t);

  return ;
}

/**
 *  Prepare an insert statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 */
template <typename T>
void destination::_prepare_insert(std::auto_ptr<soci::statement>& st,
                                  T& t) {
  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(MappedType<T>::table);
  query.append(" (");
  for (typename std::list<std::pair<std::string,
                                    GetterSetter<T> > >::const_iterator
         it = DBMappedType<T>::list.begin(),
         end = DBMappedType<T>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (typename std::list<std::pair<std::string,
                                    GetterSetter<T> > >::const_iterator
         it = DBMappedType<T>::list.begin(),
         end = DBMappedType<T>::list.end();
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
  st.reset(new soci::statement((_conn->prepare << query,
                                soci::use(t))));

  return ;
}

/**
 *  Prepare an update statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 *  @param[in]  id List of fields that form an UNIQUE.
 */
template <typename T>
void destination::_prepare_update(std::auto_ptr<soci::statement>& st,
                                  T& t,
                                  std::vector<std::string> const& id) {
  // Build query string.
  std::string query;
  query = "UPDATE ";
  query.append(MappedType<T>::table);
  query.append(" SET ");
  for (typename std::list<std::pair<std::string,
                                    GetterSetter<T> > >::const_iterator
         it = DBMappedType<T>::list.begin(),
         end = DBMappedType<T>::list.end();
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
  st.reset(new soci::statement((_conn->prepare << query,
                                soci::use(t))));

  return ;
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] event Uncasted acknowledgement.
 */
void destination::_process_acknowledgement(Events::Event const& event) {
  Events::Acknowledgement const& ack(
    *static_cast<Events::Acknowledgement const*>(&event));

  logging::info << logging::MEDIUM
                << "processing acknowledgement event";
  try {
    _insert(ack);
  }
  catch (soci::soci_error const& se) {
    _acknowledgement = ack;
    _acknowledgement_stmt->execute(true);
  }
  return ;
}

/**
 *  Process a comment event.
 *
 *  @param[in] event Uncasted comment.
 */
void destination::_process_comment(Events::Event const& event) {
  Events::comment const& c(
    *static_cast<Events::comment const*>(&event));

  logging::info << logging::MEDIUM << "processing comment event";
  try {
    _insert(c);
  }
  catch (soci::soci_error const& se) {
    _comment = c;
    _comment_stmt->execute(true);
  }
  return ;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] event Uncasted custom variable.
 */
void destination::_process_custom_variable(Events::Event const& event) {
  Events::custom_variable const& cv(
    *static_cast<Events::custom_variable const*>(&event));

  logging::info << logging::MEDIUM << "processing custom variable event";
  _insert(cv);
  return ;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] event Uncasted custom variable status.
 */
void destination::_process_custom_variable_status(Events::Event const& event) {
  Events::custom_variable_status const& cvs(
    *static_cast<Events::custom_variable_status const*>(&event));

  logging::info << logging::MEDIUM << "processing custom variable status event";
  _custom_variable_status = cvs;
  _custom_variable_status_stmt->execute(true);
  return ;
}

/**
 *  Process a downtime event.
 *
 *  @param[in] event Uncasted downtime.
 */
void destination::_process_downtime(Events::Event const& event) {
  Events::Downtime const& downtime(
    *static_cast<Events::Downtime const*>(&event));

  logging::info << logging::MEDIUM << "processing downtime event";
  try {
    _insert(downtime);
  }
  catch (soci::soci_error const& se) {
    _downtime = downtime;
    _downtime_stmt->execute(true);
  }
  return ;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] event Uncasted event handler.
 */
void destination::_process_event_handler(Events::Event const& event) {
  Events::event_handler const& event_handler(
    *static_cast<Events::event_handler const*>(&event));

  logging::info << logging::MEDIUM << "processing event handler event";
  try {
    _insert(event_handler);
  }
  catch (soci::soci_error const& se) {
    _event_handler = event_handler;
    _event_handler_stmt->execute(true);
  }
  return ;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] event Uncasted flapping status.
 */
void destination::_process_flapping_status(Events::Event const& event) {
  Events::flapping_status const& flapping_status(
    *static_cast<Events::flapping_status const*>(&event));

  logging::info << logging::MEDIUM << "processing flapping status event";
  try {
    _insert(flapping_status);
  }
  catch (soci::soci_error const& se) {
    _flapping_status = flapping_status;
    _flapping_status_stmt->execute(true);
  }
  return ;
}

/**
 *  Process an host event.
 *
 *  @param[in] event Uncasted host.
 */
void destination::_process_host(Events::Event const& event) {
  Events::Host const& host(*static_cast<Events::Host const*>(&event));

  logging::info << logging::MEDIUM << "processing host event";
  _host = host;
  _host_stmt->execute(true);
  long long matched = _host_stmt->get_affected_rows();
  if (!matched || (-1 == matched))
    _insert(host);
  return ;
}

/**
 *  Process an host check event.
 *
 *  @param[in] event Uncasted host check.
 */
void destination::_process_host_check(Events::Event const& event) {
  Events::HostCheck const& host_check(
    *static_cast<Events::HostCheck const*>(&event));

  logging::info << logging::MEDIUM << "processing host check event";
  _host_check = host_check;
  _host_check_stmt->execute(true);
  return ;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] event Uncasted host dependency.
 */
void destination::_process_host_dependency(Events::Event const& event) {
  Events::HostDependency const& hd(
    *static_cast<Events::HostDependency const*>(&event));

  logging::info << logging::MEDIUM << "processing host dependency event";
  _insert(hd);
  return ;
}

/**
 *  Process a host group event.
 *
 *  @param[in] event Uncasted host group.
 */
void destination::_process_host_group(Events::Event const& event) {
  Events::HostGroup const& hg(
    *static_cast<Events::HostGroup const*>(&event));

  logging::info << logging::MEDIUM << "processing host group event";
  _insert(hg);
  return ;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] event Uncasted host group member.
 */
void destination::_process_host_group_member(Events::Event const& event) {
  Events::HostGroupMember const& hgm(
    *static_cast<Events::HostGroupMember const*>(&event));
  int hostgroup_id;

  logging::info << logging::MEDIUM << "processing host group member event";

  // Fetch host group ID.
  *_conn << "SELECT hostgroup_id FROM "
         << MappedType<Events::HostGroup>::table
         << " WHERE instance_id=" << hgm.instance_id
         << " AND name=\"" << hgm.group << "\"",
         soci::into(hostgroup_id);

  // Execute query.
  *_conn << "INSERT INTO "
         << MappedType<Events::HostGroupMember>::table
         << " (host_id, hostgroup_id) VALUES("
         << hgm.host_id << ", "
         << hostgroup_id << ")";

  return ;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] event Uncasted host parent.
 */
void destination::_process_host_parent(Events::Event const& event) {
  Events::HostParent const& hp(
    *static_cast<Events::HostParent const*>(&event));

  logging::info << logging::MEDIUM << "processing host parent event";
  _insert(hp);
  return ;
}

/**
 *  Process a host status event.
 *
 *  @param[in] event Uncasted host status.
 */
void destination::_process_host_status(Events::Event const& event) {
  Events::HostStatus const& hs(
    *static_cast<Events::HostStatus const*>(&event));

  logging::info << logging::MEDIUM << "processing host status event";
  _host_status = hs;
  _host_status_stmt->execute(true);
  return ;
}

/**
 *  Process an issue event.
 *
 *  @param[in] event Uncasted issue.
 */
void destination::_process_issue(Events::Event const& event) {
  Events::Issue const& issue(
    *static_cast<Events::Issue const*>(&event));

  logging::info << logging::MEDIUM << "processing issue event";
  try {
    _insert(issue);
  }
  catch (soci::soci_error const& se) {
    _issue = issue;
    _issue_stmt->execute(true);
  }
  return ;
}

/**
 *  Process an issue parent event.
 *
 *  @param[in] event Uncasted issue parent.
 */
void destination::_process_issue_parent(Events::Event const& event) {
  Events::IssueParent const& ip(
    *static_cast<Events::IssueParent const*>(&event));
  int child_id;
  int parent_id;

  logging::info << logging::MEDIUM << "processing issue parent event";

  // Get child ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << MappedType<Events::Issue>::table << " WHERE host_id="
          << ip.child_host_id << " AND service_id="
          << ip.child_service_id << " AND start_time="
          << ip.child_start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    *_conn << query.str(), soci::into(child_id);
    logging::debug << logging::LOW << "child issue ID: " << child_id;
  }

  // Get parent ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << MappedType<Events::Issue>::table << " WHERE host_id="
          << ip.parent_host_id << " AND service_id="
          << ip.parent_service_id << " AND start_time="
          << ip.parent_start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    *_conn << query.str(), soci::into(parent_id);
    logging::debug << logging::LOW << "parent issue ID: " << parent_id;
  }

  if (ip.end_time) {
    std::ostringstream query;
    query << "UPDATE "
          << MappedType<Events::IssueParent>::table
          << " SET end_time="
          << ip.end_time << " WHERE child_id="
          << child_id << " AND parent_id="
          << parent_id << " AND start_time="
          << ip.start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    *_conn << query.str();
  }
  else {
    std::ostringstream query;
    query << "INSERT INTO "
          << MappedType<Events::IssueParent>::table
          << " (child_id, parent_id, start_time) VALUES("
          << child_id << ", "
          << parent_id << ", "
          << ip.start_time << ")";
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    *_conn << query.str();
  }

  return ;
}

/**
 *  Process a log event.
 *
 *  @param[in] event Uncasted log.
 */
void destination::_process_log(Events::Event const& event)
{
  char const* field;
  int issue;
  Events::Log const& log(*static_cast<Events::Log const*>(&event));
  std::string query;

  logging::info << logging::MEDIUM << "processing log event";
  field = "issue_id";
  query = "INSERT INTO ";
  query.append(MappedType<Events::Log>::table);
  query.append("(");
  for (std::list<std::pair<std::string,
                           GetterSetter<Events::Log> > >::const_iterator
         it = DBMappedType<Events::Log>::list.begin(),
         end = DBMappedType<Events::Log>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.append(field);
  query.append(") VALUES(");
  for (std::list<std::pair<std::string,
                           GetterSetter<Events::Log> > >::const_iterator
         it = DBMappedType<Events::Log>::list.begin(),
         end = DBMappedType<Events::Log>::list.end();
       it != end;
       ++it) {
    query.append(":");
    query.append(it->first);
    query.append(", ");
  }
  query.append(":");
  query.append(field);
  query.append(")");
  logging::info << logging::LOW << "executing query: " << query.c_str();

  // Fetch issue ID (if any).
  if (log.issue_start_time) {
    std::ostringstream ss;
    ss << "SELECT issue_id FROM "
       << MappedType<Events::Issue>::table
       << " WHERE host_id=" << log.host_id
       << " AND service_id=" << log.service_id
       << " AND start_time=" << log.issue_start_time;
    logging::info << logging::LOW << "executing query: "
                  << ss.str().c_str();
    *_conn << ss.str(), soci::into(issue);
  }
  else
    issue = 0;

  // Execute query.
  logging::info << logging::LOW << "executing query: " << query.c_str();
  *_conn << query, soci::use(log), soci::use(issue, field);

  return ;
}

/**
 *  Process a notification event.
 *
 *  @param[in] event Uncasted notification.
 */
void destination::_process_notification(Events::Event const& event) {
  Events::notification const& notification(
    *static_cast<Events::notification const*>(&event));

  logging::info << logging::MEDIUM << "processing notification event";
  try {
    _insert(notification);
  }
  catch (soci::soci_error const& se) {
    _notification = notification;
    _notification_stmt->execute(true);
  }
  return ;
}

/**
 *  Process a program event.
 *
 *  @param[in] event Uncasted program.
 */
void destination::_process_program(Events::Event const& event) {
  Events::Program const& program(
    *static_cast<Events::Program const*>(&event));

  logging::info << logging::MEDIUM << "processing program event";
  _clean_tables(program.instance_id);
  if (!program.program_end) {
    try {
      _insert(program);
    }
    catch (soci::soci_error const& se) {
      _program = program;
      _program_stmt->execute(true);
    }
  }
  else {
    _program = program;
    _program_stmt->execute(true);
  }
  return ;
}

/**
 *  Process a program status event.
 *
 *  @param[in] event Uncasted program status.
 */
void destination::_process_program_status(Events::Event const& event) {
  Events::ProgramStatus const& ps(
    *static_cast<Events::ProgramStatus const*>(&event));

  logging::info << logging::MEDIUM << "processing program status event";
  _program_status = ps;
  _program_status_stmt->execute(true);
  return ;
}

/**
 *  Process a service event.
 *
 *  @param[in] event Uncasted service.
 */
void destination::_process_service(Events::Event const& event) {
  Events::Service const& service(
    *static_cast<Events::Service const*>(&event));

  logging::info << logging::MEDIUM << "processing service event";
  _service = service;
  _service_stmt->execute(true);
  long long matched = _service_stmt->get_affected_rows();
  if (!matched || (-1 == matched)) {
    _service = service;
    _service_insert_stmt->execute(true);
  }
  return ;
}

/**
 *  Process a service check event.
 *
 *  @param[in] event Uncasted service check.
 */
void destination::_process_service_check(Events::Event const& event) {
  Events::ServiceCheck const& service_check(
    *static_cast<Events::ServiceCheck const*>(&event));

  logging::info << logging::MEDIUM << "processing service check event";
  _service_check = service_check;
  _service_check_stmt->execute(true);
  return ;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] event Uncasted service dependency.
 */
void destination::_process_service_dependency(Events::Event const& event) {
  Events::ServiceDependency const& sd(
    *static_cast<Events::ServiceDependency const*>(&event));

  logging::info << logging::MEDIUM << "processing ServiceDependency event";
  _insert(sd);
  return ;
}

/**
 *  Process a service group event.
 *
 *  @param[in] event Uncasted service group.
 */
void destination::_process_service_group(Events::Event const& event) {
  Events::ServiceGroup const& sg(
    *static_cast<Events::ServiceGroup const*>(&event));

  logging::info << logging::MEDIUM << "processing service group event";
  _insert(sg);
  return ;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] event Uncasted service group member.
 */
void destination::_process_service_group_member(Events::Event const& event) {
  int servicegroup_id;
  Events::ServiceGroupMember const& sgm(
    *static_cast<Events::ServiceGroupMember const*>(&event));

  logging::info << logging::MEDIUM << "processing service group member event";

  // Fetch service group ID.
  {
    std::ostringstream ss;
    ss << "SELECT servicegroup_id FROM "
       << MappedType<Events::ServiceGroup>::table
       << " WHERE instance_id=" << sgm.instance_id
       << " AND name=\"" << sgm.group << "\"";
    logging::info << logging::LOW << "executing query: "
                  << ss.str().c_str();
    *_conn << ss.str(), soci::into(servicegroup_id);
  }

  // Execute query.
  {
    std::ostringstream ss;
    ss << "INSERT INTO "
       << MappedType<Events::ServiceGroupMember>::table
       << " (host_id, service_id, servicegroup_id) VALUES("
       << sgm.host_id << ", "
       << sgm.service_id << ", "
       << servicegroup_id << ")";
    logging::info << logging::LOW << "executing query: "
                  << ss.str().c_str();
    *_conn << ss.str();
  }

  return ;
}

/**
 *  Process a service status event.
 *
 *  @param[in] event Uncasted service status.
 */
void destination::_process_service_status(Events::Event const& event) {
  Events::ServiceStatus const& ss(
    *static_cast<Events::ServiceStatus const*>(&event));

  logging::info << logging::MEDIUM << "processing service status event";
  _service_status = ss;
  _service_status_stmt->execute(true);
  return ;
}

/**
 *  Process a state event.
 *
 *  @param[in] event Uncasted state.
 */
void destination::_process_state(Events::Event const& event) {
  Events::state const& s(
    *static_cast<Events::state const*>(&event));

  logging::info << logging::MEDIUM << "processing state event";
  if (s.end_time) {
    _state = s;
    _state_stmt->execute(true);
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
         == Events::Event::EVENT_TYPES_NB);
}

/**
 *  @brief Destination destructor.
 *
 *  Release all previously allocated ressources.
 */
destination::~destination() {
  Close();
}

/**
 *  Close the event destination.
 */
void destination::Close()
{
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
 *  @param[in] event Event that should be stored in the database.
 */
void destination::Event(Events::Event* event) {
  try {
    (this->*this->processing_table[event->GetType()])(*event);
  }
  catch (...) {
    // Event self deregistration.
    event->RemoveReader();

    // Rethrow the exception
    throw ;
  }

  // Event self deregistration.
  event->RemoveReader();

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
  // Connect to DB.
  {
    std::stringstream ss;

    switch (db_type) {
#ifdef USE_MYSQL
     case MYSQL:
      ss << "dbname=" << db
         << " host=" << host
         << " user=" << user
         << " password=" << pass;
      _conn.reset(new soci::session(soci::mysql, ss.str()));
      break ;
#endif /* USE_MYSQL */

#ifdef USE_ORACLE
     case ORACLE:
      ss << "service=" << host
         << " user=" << user
         << " password=" << pass;
      _conn.reset(new soci::session(soci::oracle, ss.str()));
      break ;
#endif /* USE_ORACLE */

#ifdef USE_POSTGRESQL
     case POSTGRESQL:
      ss << "dbname=" << db
         << " host=" << host
         << " user=" << user
         << " password=" << pass;
      _conn.reset(new soci::session(soci::postgresql, ss.str()));
      break ;
#endif /* USE_POSTGRESQL */

     default:
      throw Exception(0, "Unsupported DBMS requested.");
    }
  }

  _prepare_insert<Events::Service>(_service_insert_stmt, _service);

  std::vector<std::string> id;

  id.clear();
  id.push_back("author_name");
  id.push_back("entry_time");
  id.push_back("host_name");
  id.push_back("instance_name");
  id.push_back("service_description");
  _prepare_update<Events::Acknowledgement>(
    _acknowledgement_stmt, _acknowledgement, id);

  id.clear();
  id.push_back("entry_time");
  id.push_back("instance_name");
  id.push_back("internal_id");
  _prepare_update<Events::comment>(
    _comment_stmt, _comment, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("name");
  id.push_back("service_id");
  _prepare_update<Events::custom_variable_status>(
    _custom_variable_status_stmt, _custom_variable_status, id);

  id.clear();
  id.push_back("entry_time");
  id.push_back("instance_name");
  id.push_back("internal_id");
  _prepare_update<Events::Downtime>(
    _downtime_stmt, _downtime, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<Events::event_handler>(
    _event_handler_stmt, _event_handler, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("event_time");
  _prepare_update<Events::flapping_status>(
    _flapping_status_stmt, _flapping_status, id);

  id.clear();
  id.push_back("host_id");
  _prepare_update<Events::Host>(
    _host_stmt, _host, id);

  id.clear();
  id.push_back("host_id");
  _prepare_update<Events::HostCheck>(
    _host_check_stmt, _host_check, id);

  id.clear();
  id.push_back("host_id");
  _prepare_update<Events::HostStatus>(
    _host_status_stmt, _host_status, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<Events::Issue>(
    _issue_stmt, _issue, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<Events::notification>(
    _notification_stmt, _notification, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  id.push_back("start_time");
  _prepare_update<Events::state>(
    _state_stmt, _state, id);

  id.clear();
  id.push_back("instance_id");
  _prepare_update<Events::Program>(
    _program_stmt, _program, id);

  id.clear();
  id.push_back("instance_id");
  _prepare_update<Events::ProgramStatus>(
    _program_status_stmt, _program_status, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  _prepare_update<Events::Service>(
    _service_stmt, _service, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  _prepare_update<Events::ServiceCheck>(
    _service_check_stmt, _service_check, id);

  id.clear();
  id.push_back("host_id");
  id.push_back("service_id");
  _prepare_update<Events::ServiceStatus>(
    _service_status_stmt, _service_status, id);

  return ;
}
