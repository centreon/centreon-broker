/*
** Copyright 2009-2011 Merethis
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

#include <assert.h>
#include <QPair>
#include <QSqlError>
#include <QVariant>
#include <QVector>
#include <sstream>
#include <stdlib.h>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "mapping.hh"
#include "com/centreon/broker/sql/internal.hh"
#include "com/centreon/broker/sql/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Processing table.
QHash<QString, void (stream::*)(io::data const&)> stream::processing_table;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Assignment operator.
 *
 *  Should not be used. Any call to this method will result in a call to
 *  abort().
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  (void)s;
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
void stream::_clean_tables(int instance_id) {
  // Disable hosts and services.
  {
    std::ostringstream ss;
    ss << "UPDATE " << mapped_type<neb::host>::table
       << " JOIN " << mapped_type<neb::service>::table << " ON "
       << mapped_type<neb::host>::table << ".host_id="
       << mapped_type<neb::service>::table << ".host_id SET "
       << mapped_type<neb::host>::table << ".enabled=0, "
       << mapped_type<neb::service>::table << ".enabled=0"
       << " WHERE " << mapped_type<neb::host>::table
       << ".instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove host groups.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::host_group>::table
       << " WHERE instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove service groups.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::service_group>::table
       << " WHERE instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove host dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::host_dependency>::table
       << " WHERE host_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")"
          " OR dependent_host_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")";
    _execute(ss.str().c_str());
  }

  // Remove host parents.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::host_parent>::table
       << " WHERE child_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")"
          " OR parent_id IN ("
          "  SELECT host_id"
          "   FROM " << mapped_type<neb::host>::table
       << "   WHERE instance_id=" << instance_id << ")";
    _execute(ss.str().c_str());
  }

  // Remove service dependencies.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::service_dependency>::table
       << " WHERE service_id IN ("
          "  SELECT services.service_id"
          "   FROM " << mapped_type<neb::service>::table << " AS services"
       << "   JOIN " << mapped_type<neb::host>::table << " AS hosts"
       << "   ON hosts.host_id=services.service_id WHERE hosts.instance_id="
       << instance_id << ")"
          " OR dependent_service_id IN ("
          "  SELECT services.service_id "
          "   FROM " << mapped_type<neb::service>::table << " AS services"
          "   JOIN " << mapped_type<neb::host>::table << " AS hosts"
          "   ON hosts.host_id=services.service_id WHERE hosts.instance_id="
       << instance_id << ")";
    _execute(ss.str().c_str());
  }

  // Remove list of modules.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::module>::table
       << " WHERE instance_id=" << instance_id;
    _execute(ss.str().c_str());
  }

  // Remove custom variables.
  {
    std::ostringstream ss;
    ss << "DELETE FROM " << mapped_type<neb::custom_variable>::table
       << " USING " << mapped_type<neb::custom_variable>::table
       << " JOIN "  << mapped_type<neb::host>::table << " ON "
       << mapped_type<neb::custom_variable>::table << ".host_id="
       << mapped_type<neb::host>::table << ".host_id" << " WHERE "
       << mapped_type<neb::host>::table << ".instance_id="
       << instance_id;
    _execute(ss.str().c_str());
  }

  return ;
}

/**
 *  Execute a plain SQL query.
 *
 *  @param[in] query Query to execute.
 */
void stream::_execute(QString const& query) {
  logging::debug << logging::LOW << "SQL: executing query: " << query;
  _db.exec(query);
  QSqlError err(_db.lastError());
  if (err.type() != QSqlError::NoError)
    throw (exceptions::msg() << "SQL: " << err.text());
  return ;
}

/**
 *  Execute a prepare SQL query.
 *
 *  @param[in] query Query to execute.
 */
void stream::_execute(QSqlQuery& query) {
  logging::debug << logging::LOW << "SQL: executing query";
  if (!query.exec())
    throw (exceptions::msg() << "SQL: " << query.lastError().text());
  return ;
}

/**
 *  Insert an object in the DB using its mapping.
 *
 *  @param[in] t Object to insert.
 */
template <typename T>
bool stream::_insert(T const& t) {
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

  // Execute query.
  QSqlQuery q(_db);
  bool ret(q.prepare(query.c_str()));
  if (ret) {
    q << t;
    ret = q.exec();
  }

  return (ret);
}

/**
 *  Prepare queries.
 */
void stream::_prepare() {
  // Prepare insert queries.
  _prepare_insert<neb::custom_variable>(_custom_variable_insert_stmt);
  _prepare_insert<neb::service>(_service_insert_stmt);

  // Prepare update queries.
  QVector<QPair<QString, bool> > id;

  id.clear();
  id.push_back(qMakePair(QString("entry_time"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::acknowledgement>(_acknowledgement_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("entry_time"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::comment>(_comment_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("name"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::custom_variable_status>(_custom_variable_status_stmt,
    id);

  id.clear();
  id.push_back(qMakePair(QString("entry_time"), false));
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  _prepare_update<neb::downtime>(_downtime_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<neb::event_handler>(_event_handler_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("event_time"), false));
  _prepare_update<neb::flapping_status>(_flapping_status_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  _prepare_update<neb::host>(_host_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  _prepare_update<neb::host_check>(_host_check_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  _prepare_update<neb::host_status>(_host_status_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("instance_id"), false));
  _prepare_update<neb::instance>(_instance_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("instance_id"), false));
  _prepare_update<neb::instance_status>(_instance_status_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<neb::notification>(_notification_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service>(_service_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service_check>(_service_check_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  _prepare_update<neb::service_status>(_service_status_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<correlation::host_state>(_host_state_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), true));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<correlation::issue>(_issue_stmt, id);

  id.clear();
  id.push_back(qMakePair(QString("host_id"), false));
  id.push_back(qMakePair(QString("service_id"), false));
  id.push_back(qMakePair(QString("start_time"), false));
  _prepare_update<correlation::service_state>(_service_state_stmt, id);

  return ;
}

/**
 *  Prepare an insert statement for later execution.
 *
 *  @param[out] st Statement object.
 *  @param[in]  t  Object that will be bound to the statement.
 */
template <typename T>
bool stream::_prepare_insert(std::auto_ptr<QSqlQuery>& st) {
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
  st.reset(new QSqlQuery(_db));
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
bool stream::_prepare_update(std::auto_ptr<QSqlQuery>& st,
                             QVector<QPair<QString, bool> > const& id) {
  // Build query string.
  std::string query;
  query = "UPDATE ";
  query.append(mapped_type<T>::table);
  query.append(" SET ");
  for (typename std::list<std::pair<std::string, getter_setter<T> > >::const_iterator
         it = db_mapped_type<T>::list.begin(),
         end = db_mapped_type<T>::list.end();
       it != end;
       ++it) {
    bool found(false);
    for (QVector<QPair<QString, bool> >::const_iterator
           it2 = id.begin(), end2 = id.end();
         it2 != end2;
         ++it2)
      if (it->first == it2->first.toStdString())
        found = true;
    if (!found) {
      query.append(it->first);
      query.append("=:");
      query.append(it->first);
      query.append(", ");
    }
  }
  query.resize(query.size() - 2);
  query.append(" WHERE ");
  for (QVector<QPair<QString, bool> >::const_iterator
         it = id.begin(), end = id.end();
       it != end;
       ++it) {
    if (it->second) {
      query.append("COALESCE(");
      query.append(it->first.toStdString());
      query.append(", -1)=COALESCE(:");
      query.append(it->first.toStdString());
      query.append(", -1)");
    }
    else {
      query.append(it->first.toStdString());
      query.append("=:");
      query.append(it->first.toStdString());
    }
    query.append(" AND ");
  }
  query.resize(query.size() - 5);
  logging::info << logging::LOW << "SQL: preparing statement: "
                << query.c_str();

  // Prepare statement.
  st.reset(new QSqlQuery(_db));
  return (st->prepare(query.c_str()));
}

/**
 *  Process an acknowledgement event.
 *
 *  @param[in] e Uncasted acknowledgement.
 */
void stream::_process_acknowledgement(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing acknowledgement event";

  // Processing.
  neb::acknowledgement const& ack(
    *static_cast<neb::acknowledgement const*>(&e));
  if (!_insert(ack)) {
    *_acknowledgement_stmt << ack;
    _execute(*_acknowledgement_stmt);
  }

  return ;
}

/**
 *  Process a comment event.
 *
 *  @param[in] e Uncasted comment.
 */
void stream::_process_comment(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing comment event";

  // Processing.
  neb::comment const& c(*static_cast<neb::comment const*>(&e));
  if (!_insert(c)) {
    *_comment_stmt << c;
    _execute(*_comment_stmt);
  }

  return ;
}

/**
 *  Process a custom variable event.
 *
 *  @param[in] e Uncasted custom variable.
 */
void stream::_process_custom_variable(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing custom variable event";

  // Processing.
  neb::custom_variable const& cv(
    *static_cast<neb::custom_variable const*>(&e));
  *_custom_variable_insert_stmt << cv;
  _execute(*_custom_variable_insert_stmt);

  return ;
}

/**
 *  Process a custom variable status event.
 *
 *  @param[in] e Uncasted custom variable status.
 */
void stream::_process_custom_variable_status(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing custom variable status event";

  // Processing.
  neb::custom_variable_status const& cvs(
    *static_cast<neb::custom_variable_status const*>(&e));
  *_custom_variable_status_stmt << cvs;
  _execute(*_custom_variable_status_stmt);

  return ;
}

/**
 *  Process a downtime event.
 *
 *  @param[in] e Uncasted downtime.
 */
void stream::_process_downtime(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing downtime event";

  // Processing.
  neb::downtime const& d(*static_cast<neb::downtime const*>(&e));
  if (!_insert(d)) {
    *_downtime_stmt << d;
    _execute(*_downtime_stmt);
  }

  return ;
}

/**
 *  Process an event handler event.
 *
 *  @param[in] e Uncasted event handler.
 */
void stream::_process_event_handler(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing event handler event";

  // Processing.
  neb::event_handler const& eh(
    *static_cast<neb::event_handler const*>(&e));
  if (!_insert(eh)) {
    *_event_handler_stmt << eh;
    _execute(*_event_handler_stmt);
  }

  return ;
}

/**
 *  Process a flapping status event.
 *
 *  @param[in] e Uncasted flapping status.
 */
void stream::_process_flapping_status(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing flapping status event";

  // Processing.
  neb::flapping_status const& fs(
    *static_cast<neb::flapping_status const*>(&e));
  if (!_insert(fs)) {
    *_flapping_status_stmt << fs;
    _execute(*_flapping_status_stmt);
  }

  return ;
}

/**
 *  Process an host event.
 *
 *  @param[in] e Uncasted host.
 */
void stream::_process_host(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host event";

  // Processing.
  neb::host const& h(*static_cast<neb::host const*>(&e));
  *_host_stmt << h;
  _execute(*_host_stmt);
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
void stream::_process_host_check(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host check event";

  // Processing.
  neb::host_check const& hc(
    *static_cast<neb::host_check const*>(&e));
  *_host_check_stmt << hc;
  _execute(*_host_check_stmt);

  return ;
}

/**
 *  Process a host dependency event.
 *
 *  @param[in] e Uncasted host dependency.
 */
void stream::_process_host_dependency(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host dependency event";

  // Processing.
  neb::host_dependency const& hd(
    *static_cast<neb::host_dependency const*>(&e));
  _insert(hd);

  return ;
}

/**
 *  Process a host group event.
 *
 *  @param[in] e Uncasted host group.
 */
void stream::_process_host_group(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host group event";

  // Processing.
  neb::host_group const& hg(
    *static_cast<neb::host_group const*>(&e));
  _insert(hg);

  return ;
}

/**
 *  Process a host group member event.
 *
 *  @param[in] e Uncasted host group member.
 */
void stream::_process_host_group_member(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host group member event";

  // Fetch proper structure.
  neb::host_group_member const& hgm(
    *static_cast<neb::host_group_member const*>(&e));

  // Fetch host group ID.
  std::ostringstream ss;
  ss << "SELECT hostgroup_id FROM "
     << mapped_type<neb::host_group>::table
     << " WHERE instance_id=" << hgm.instance_id
     << " AND name=\"" << hgm.group.toStdString() << "\"";
  QSqlQuery q(_db);
  logging::info << logging::LOW << "executing query: "
                << ss.str().c_str();
  if (q.exec(ss.str().c_str()) && q.next()) {
    // Fetch hostgroup ID.
    int hostgroup_id(q.value(0).toInt());
    logging::debug << logging::MEDIUM
      << "SQL: fetch hostgroup of id " << hostgroup_id;

    // Insert hostgroup membership.
    std::ostringstream oss;
    oss << "INSERT INTO "
        << mapped_type<neb::host_group_member>::table
        << " (host_id, hostgroup_id) VALUES("
        << hgm.host_id << ", "
        << hostgroup_id << ")";
    logging::info << logging::LOW << "SQL: executing query: "
      << oss.str().c_str();
    _db.exec(oss.str().c_str());
  }
  else
    logging::info << logging::HIGH
      << "SQL: discarding membership between host " << hgm.host_id
      << " and hostgroup (" << hgm.instance_id << ", " << hgm.group
      << ")";

  return ;
}

/**
 *  Process a host parent event.
 *
 *  @param[in] e Uncasted host parent.
 */
void stream::_process_host_parent(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host parent event";

  // Processing.
  neb::host_parent const& hp(
    *static_cast<neb::host_parent const*>(&e));
  _insert(hp);

  return ;
}

/**
 *  Process a host state event.
 *
 *  @param[in] e Uncasted host state.
 */
void stream::_process_host_state(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host state event";

  // Processing.
  correlation::host_state const& hs(
    *static_cast<correlation::host_state const*>(&e));
  if (hs.end_time) {
    *_host_state_stmt << hs;
    _execute(*_host_state_stmt);
  }
  else
    _insert(hs);

  return ;
}

/**
 *  Process a host status event.
 *
 *  @param[in] e Uncasted host status.
 */
void stream::_process_host_status(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing host status event";

  // Processing.
  neb::host_status const& hs(
    *static_cast<neb::host_status const*>(&e));
  *_host_status_stmt << hs;
  _execute(*_host_status_stmt);

  return ;
}

/**
 *  Process an instance event.
 *
 *  @param[in] e Uncasted instance.
 */
void stream::_process_instance(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing instance event";

  // Clean tables.
  neb::instance const& i(*static_cast<neb::instance const*>(&e));
  _clean_tables(i.id);

  // Program start.
  if (!i.program_end) {
    if (!_insert(i)) {
      *_instance_stmt << i;
      _execute(*_instance_stmt);
    }
  }
  // Program end.
  else {
    *_instance_stmt << i;
    _execute(*_instance_stmt);
  }

  return ;
}

/**
 *  Process an instance status event.
 *
 *  @param[in] e Uncasted instance status.
 */
void stream::_process_instance_status(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing instance status event";

  // Processing.
  neb::instance_status const& is(
    *static_cast<neb::instance_status const*>(&e));
  *_instance_status_stmt << is;
  _execute(*_instance_status_stmt);

  return ;
}

/**
 *  Process an issue event.
 *
 *  @param[in] e Uncasted issue.
 */
void stream::_process_issue(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing issue event";

  // Processing.
  correlation::issue const& i(*static_cast<correlation::issue const*>(&e));
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
void stream::_process_issue_parent(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing issue parent event";

  // Fetch proper structure.
  correlation::issue_parent const& ip(
    *static_cast<correlation::issue_parent const*>(&e));
  int child_id;
  int parent_id;

  // Get child ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << mapped_type<correlation::issue>::table << " WHERE host_id="
          << ip.child_host_id << " AND service_id="
          << ip.child_service_id << " AND start_time="
          << ip.child_start_time;
    logging::info << logging::LOW << "executing query: "
                  << query.str().c_str();
    QSqlQuery q(_db);
    if (q.exec(query.str().c_str()) && q.next()) {
      child_id = q.value(0).toInt();
      logging::debug << logging::LOW << "SQL: child issue ID: "
        << child_id;
    }
    else
      throw (exceptions::msg() << "SQL: could not fetch child issue " \
               << "ID (host=" << ip.child_host_id << ", service="
               << ip.child_service_id << ", start="
               << ip.child_start_time << ")");
  }

  // Get parent ID.
  {
    std::ostringstream query;
    query << "SELECT issue_id FROM "
          << mapped_type<correlation::issue>::table << " WHERE host_id="
          << ip.parent_host_id << " AND service_id="
          << ip.parent_service_id << " AND start_time="
          << ip.parent_start_time;
    logging::info << logging::LOW << "SQL: executing query: "
      << query.str().c_str();
    QSqlQuery q(_db);
    if (q.exec(query.str().c_str()) && q.next()) {
      parent_id = q.value(0).toInt();
      logging::debug << logging::LOW << "SQL: parent issue ID: "
        << parent_id;
    }
    else
      throw (exceptions::msg() << "SQL: could not fetch parent issue " \
               << "ID (host=" << ip.parent_host_id << ", service="
               << ip.parent_service_id << ", start="
               << ip.parent_start_time << ")");
  }

  // End of parenting.
  if (ip.end_time) {
    std::ostringstream query;
    query << "UPDATE "
          << mapped_type<correlation::issue_parent>::table
          << " SET end_time="
          << ip.end_time << " WHERE child_id="
          << child_id << " AND parent_id="
          << parent_id << " AND start_time="
          << ip.start_time;
    logging::info << logging::LOW << "SQL: executing query: "
                  << query.str().c_str();
    _db.exec(query.str().c_str());
  }
  // New parenting.
  else {
    std::ostringstream query;
    query << "INSERT INTO "
          << mapped_type<correlation::issue_parent>::table
          << " (child_id, parent_id, start_time) VALUES("
          << child_id << ", "
          << parent_id << ", "
          << ip.start_time << ")";
    logging::info << logging::LOW << "SQL: executing query: "
                  << query.str().c_str();
    _db.exec(query.str().c_str());
  }

  return ;
}

/**
 *  Process a log event.
 *
 *  @param[in] e Uncasted log.
 */
void stream::_process_log(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing log event";

  // Fetch proper structure.
  neb::log_entry const& le(
    *static_cast<neb::log_entry const*>(&e));

  // Fetch issue ID (if any).
  int issue;
  if (le.issue_start_time) {
    std::ostringstream ss;
    ss << "SELECT issue_id FROM "
       << mapped_type<correlation::issue>::table
       << " WHERE host_id=" << le.host_id
       << " AND service_id=" << le.service_id
       << " AND start_time=" << le.issue_start_time;
    logging::info << logging::LOW << "SQL: executing query: "
      << ss.str().c_str();
    QSqlQuery q(_db);
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
  query.append(mapped_type<neb::log_entry>::table);
  query.append("(");
  for (std::list<std::pair<std::string, getter_setter<neb::log_entry> > >::const_iterator
         it = db_mapped_type<neb::log_entry>::list.begin(),
         end = db_mapped_type<neb::log_entry>::list.end();
       it != end;
       ++it) {
    query.append(it->first);
    query.append(", ");
  }
  query.append(field);
  query.append(") VALUES(");
  for (std::list<std::pair<std::string, getter_setter<neb::log_entry> > >::const_iterator
         it = db_mapped_type<neb::log_entry>::list.begin(),
         end = db_mapped_type<neb::log_entry>::list.end();
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
  logging::info << logging::LOW
    << "SQL: executing query: " << query.c_str();
  QSqlQuery q(_db);
  q.prepare(query.c_str());
  q << le;
  q.bindValue(field, issue);
  _execute(q);

  return ;
}

/**
 *  Process a module event.
 *
 *  @param[in] e Uncasted module.
 */
void stream::_process_module(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing module event";

  // Processing.
  neb::module const& m(*static_cast<neb::module const*>(&e));
  _insert(m);

  return ;
}

/**
 *  Process nothing.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_nothing(io::data const& e) {
  (void)e;
  return ;
}

/**
 *  Process a notification event.
 *
 *  @param[in] e Uncasted notification.
 */
void stream::_process_notification(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing notification event";

  // Processing.
  neb::notification const& n(
    *static_cast<neb::notification const*>(&e));
  if (!_insert(n)) {
    *_notification_stmt << n;
    _execute(*_notification_stmt);
  }

  return ;
}

/**
 *  Process a service event.
 *
 *  @param[in] e Uncasted service.
 */
void stream::_process_service(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM << "SQL: processing service event";

  // Processing.
  neb::service const& s(*static_cast<neb::service const*>(&e));
  *_service_stmt << s;
  _execute(*_service_stmt);
  int matched = _service_stmt->numRowsAffected();
  if (!matched || (-1 == matched)) {
    *_service_insert_stmt << s;
    _execute(*_service_insert_stmt);
  }

  return ;
}

/**
 *  Process a service check event.
 *
 *  @param[in] e Uncasted service check.
 */
void stream::_process_service_check(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service check event";

  // Processing.
  neb::service_check const& sc(
    *static_cast<neb::service_check const*>(&e));
  *_service_check_stmt << sc;
  _execute(*_service_check_stmt);

  return ;
}

/**
 *  Process a service dependency event.
 *
 *  @param[in] e Uncasted service dependency.
 */
void stream::_process_service_dependency(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service dependency event";

  // Processing.
  neb::service_dependency const& sd(
    *static_cast<neb::service_dependency const*>(&e));
  _insert(sd);

  return ;
}

/**
 *  Process a service group event.
 *
 *  @param[in] e Uncasted service group.
 */
void stream::_process_service_group(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service group event";

  // Processing.
  neb::service_group const& sg(
    *static_cast<neb::service_group const*>(&e));
  _insert(sg);

  return ;
}

/**
 *  Process a service group member event.
 *
 *  @param[in] e Uncasted service group member.
 */
void stream::_process_service_group_member(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service group member event";

  // Fetch proper structure.
  neb::service_group_member const& sgm(
    *static_cast<neb::service_group_member const*>(&e));

  // Fetch service group ID.
  std::ostringstream ss;
  ss << "SELECT servicegroup_id FROM "
     << mapped_type<neb::service_group>::table
     << " WHERE instance_id=" << sgm.instance_id
     << " AND name=\"" << sgm.group.toStdString() << "\"";
  QSqlQuery q(_db);
  logging::info << logging::LOW << "SQL: executing query: "
    << ss.str().c_str();
  if (q.exec(ss.str().c_str()) && q.next()) {
    // Fetch servicegroup ID.
    int servicegroup_id(q.value(0).toInt());
    logging::debug << logging::MEDIUM
      << "SQL: fetch servicegroup of id " << servicegroup_id;

    // Insert servicegroup membership.
    std::ostringstream oss;
    oss << "INSERT INTO "
        << mapped_type<neb::service_group_member>::table
        << " (host_id, service_id, servicegroup_id) VALUES("
        << sgm.host_id << ", "
        << sgm.service_id << ", "
        << servicegroup_id << ")";
    logging::info << logging::LOW << "SQL: executing query: "
      << oss.str().c_str();
    _db.exec(oss.str().c_str());
  }
  else
    logging::info << logging::HIGH
      << "SQL: discarding membership between service ("
      << sgm.host_id << ", " << sgm.service_id << ") and servicegroup ("
      << sgm.instance_id << ", " << sgm.group << ")";

  return ;
}

/**
 *  Process a service state event.
 *
 *  @param[in] e Uncasted service state.
 */
void stream::_process_service_state(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service state event";

  // Processing.
  correlation::service_state const& ss(
    *static_cast<correlation::service_state const*>(&e));
  if (ss.end_time) {
    *_service_state_stmt << ss;
    _execute(*_service_state_stmt);
  }
  else
    _insert(ss);

  return ;
}

/**
 *  Process a service status event.
 *
 *  @param[in] e Uncasted service status.
 */
void stream::_process_service_status(io::data const& e) {
  // Log message.
  logging::info << logging::MEDIUM
    << "SQL: processing service status event";

  // Processing.
  neb::service_status const& ss(
    *static_cast<neb::service_status const*>(&e));
  *_service_status_stmt << ss;
  _execute(*_service_status_stmt);

  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] type     Database type.
 *  @param[in] host     Database host.
 *  @param[in] port     Database port.
 *  @param[in] user     User.
 *  @param[in] password Password.
 *  @param[in] db       Database name.
 */
stream::stream(QString const& type,
               QString const& host,
               unsigned short port,
               QString const& user,
               QString const& password,
               QString const& db) {
  // Get the driver ID.
  QString t;
  if (!type.compare("db2", Qt::CaseInsensitive))
    t = "QDB2";
  else if (!type.compare("ibase", Qt::CaseInsensitive)
           || !type.compare("interbase", Qt::CaseInsensitive))
    t = "QIBASE";
  else if (!type.compare("mysql", Qt::CaseInsensitive))
    t = "QMYSQL";
  else if (!type.compare("oci", Qt::CaseInsensitive)
           || !type.compare("oracle", Qt::CaseInsensitive))
    t = "QOCI";
  else if (!type.compare("odbc", Qt::CaseInsensitive))
    t = "QODBC";
  else if (!type.compare("psql", Qt::CaseInsensitive)
           || !type.compare("postgres", Qt::CaseInsensitive)
           || !type.compare("postgresql", Qt::CaseInsensitive))
    t = "QPSQL";
  else if (!type.compare("sqlite", Qt::CaseInsensitive))
    t = "QSQLITE";
  else if (!type.compare("tds", Qt::CaseInsensitive)
           || !type.compare("sybase", Qt::CaseInsensitive))
    t = "QTDS";
  else
    t = type;

  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Add database connection.
  _db = QSqlDatabase::addDatabase(t, id);
  if (t == "QMYSQL")
    _db.setConnectOptions("CLIENT_FOUND_ROWS");

  // Open database.
  _db.setHostName(host);
  _db.setPort(port);
  _db.setUserName(user);
  _db.setPassword(password);
  _db.setDatabaseName(db);
  if (!_db.open())
    throw (exceptions::msg() << "SQL: could not open SQL database");

  // Prepare queries.
  _prepare();
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) : io::stream(s) {
  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Clone database.
  _db = QSqlDatabase::cloneDatabase(s._db, id);

  // Open database.
  if (!_db.open())
    throw (exceptions::msg() << "SQL: could not open SQL database");

  // Prepare queries.
  _prepare();
}

/**
 *  Destructor.
 */
stream::~stream() {
  // Connection ID.
  QString id;
  id.setNum((qulonglong)this, 16);

  // Reset statements.
  _acknowledgement_stmt.reset();
  _comment_stmt.reset();
  _custom_variable_insert_stmt.reset();
  _custom_variable_status_stmt.reset();
  _downtime_stmt.reset();
  _event_handler_stmt.reset();
  _flapping_status_stmt.reset();
  _host_stmt.reset();
  _host_check_stmt.reset();
  _host_state_stmt.reset();
  _host_status_stmt.reset();
  _instance_stmt.reset();
  _instance_status_stmt.reset();
  _issue_stmt.reset();
  _notification_stmt.reset();
  _service_insert_stmt.reset();
  _service_stmt.reset();
  _service_check_stmt.reset();
  _service_state_stmt.reset();
  _service_status_stmt.reset();

  // Remove database connection.
  QSqlDatabase::removeDatabase(id);
}

/**
 *  Initialize SQL layer.
 */
void stream::initialize() {
  // Fill processing table.
  processing_table["com::centreon::broker::neb::acknowledgement"]
    = &stream::_process_acknowledgement;
  processing_table["com::centreon::broker::neb::comment"]
    = &stream::_process_comment;
  processing_table["com::centreon::broker::neb::custom_variable"]
    = &stream::_process_custom_variable;
  processing_table["com::centreon::broker::neb::custom_variable_status"]
    = &stream::_process_custom_variable_status;
  processing_table["com::centreon::broker::neb::downtime"]
    = &stream::_process_downtime;
  processing_table["com::centreon::broker::neb::event_handler"]
    = &stream::_process_event_handler;
  processing_table["com::centreon::broker::neb::flapping_status"]
    = &stream::_process_flapping_status;
  processing_table["com::centreon::broker::neb::host"]
    = &stream::_process_host;
  processing_table["com::centreon::broker::neb::host_check"]
    = &stream::_process_host_check;
  processing_table["com::centreon::broker::neb::host_dependency"]
    = &stream::_process_host_dependency;
  processing_table["com::centreon::broker::neb::host_group"]
    = &stream::_process_host_group;
  processing_table["com::centreon::broker::neb::host_group_member"]
    = &stream::_process_host_group_member;
  processing_table["com::centreon::broker::neb::host_parent"]
    = &stream::_process_host_parent;
  processing_table["com::centreon::broker::neb::host_status"]
    = &stream::_process_host_status;
  processing_table["com::centreon::broker::neb::instance"]
    = &stream::_process_instance;
  processing_table["com::centreon::broker::neb::instance_status"]
    = &stream::_process_instance_status;
  processing_table["com::centreon::broker::neb::log_entry"]
    = &stream::_process_log;
  processing_table["com::centreon::broker::neb::module"]
    = &stream::_process_module;
  processing_table["com::centreon::broker::neb::notification"]
    = &stream::_process_notification;
  processing_table["com::centreon::broker::neb::service"]
    = &stream::_process_service;
  processing_table["com::centreon::broker::neb::service_check"]
    = &stream::_process_service_check;
  processing_table["com::centreon::broker::neb::service_dependency"]
    = &stream::_process_service_dependency;
  processing_table["com::centreon::broker::neb::service_group"]
    = &stream::_process_service_group;
  processing_table["com::centreon::broker::neb::service_group_member"]
    = &stream::_process_service_group_member;
  processing_table["com::centreon::broker::neb::service_status"]
    = &stream::_process_service_status;
  processing_table["com::centreon::broker::correlation::host_state"]
    = &stream::_process_host_state;
  processing_table["com::centreon::broker::correlation::issue"]
    = &stream::_process_issue;
  processing_table["com::centreon::broker::correlation::issue_parent"]
    = &stream::_process_issue_parent;
  processing_table["com::centreon::broker::correlation::service_state"]
    = &stream::_process_service_state;
  processing_table.squeeze();
  return ;
}

/**
 *  Read from the database.
 *
 *  @return Does not return, throw an exception.
 */
QSharedPointer<io::data> stream::read() {
  throw (exceptions::msg()
           << "SQL: attempt to read from a SQL stream (software bug)");
  return (QSharedPointer<io::data>());
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 */
void stream::write(QSharedPointer<io::data> data) {
  QHash<QString, void (stream::*)(io::data const&)>::const_iterator it;
  it = processing_table.find(data->type());
  if (it != processing_table.end())
    (this->*(it.value()))(*data);
  return ;
}
