/*
** Copyright 2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include <cassert>
#include <cstdlib>
#include <QSqlError>
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/exceptions/msg.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] db  Database object.
 */
database_query::database_query(database& db)
  : _db(db), _q(db.get_qt_db()) {
  _q.setForwardOnly(true);
}

/**
 *  Destructor.
 */
database_query::~database_query() {}

/**
 *  Set the placeholder to the value in the prepared statement.
 *
 *  @param[in] placeholder  Placeholder in the prepared statement.
 *  @param[in] value        Value.
 */
void database_query::bind_value(
                       QString const& placeholder,
                       QVariant const& value) {
  _q.bindValue(placeholder, value);
  return ;
}

/**
 *  Tells that this query won't require anymore data.
 */
void database_query::finish() {
#if QT_VERSION >= 0x040302
  _q.finish();
#endif // Qt >= 4.3.2
  return ;
}

/**
 *  Get the last inserted ID.
 *
 *  @return Last inserted ID if feature is supported.
 */
QVariant database_query::last_insert_id() {
  return (_q.lastInsertId());
}

/**
 *  Get the number of rows affected by the last query.
 *
 *  @return Number of rows affected by the last query, a negative value
 *          if it could not be determined.
 */
int database_query::num_rows_affected() {
  return (_q.numRowsAffected());
}

/**
 *  Get the number of rows returned by the last select.
 *
 *  @return Number of rows returned by the last select, a negative value
 *          if it could not be determined.
 */
int database_query::size() {
  return (_q.size());
}

/**
 *  Execute a query.
 *
 *  @param[in] query      Query to run on the database.
 *  @param[in] error_msg  Error message in case of failure.
 */
void database_query::run_query(
                       std::string const& query,
                       char const* error_msg) {
  if (!_q.exec(query.c_str())) {
    exceptions::msg e;
    if (error_msg)
      e << error_msg << ": ";
    e << "could not execute query: "
      << _q.lastError().text() << " (" << query << ")";
    throw (e);
  }
  _db.query_executed();
  return ;
}

/**
 *  Execute the prepared statement.
 *
 *  @param[in] error_msg  Error message in case of failure.
 */
void database_query::run_statement(char const* error_msg) {
  if (!_q.exec()) {
    exceptions::msg e;
    if (error_msg)
      e << error_msg << ": ";
    e << "could not execute prepared statement: "
      << _q.lastError().text();
    throw (e);
  }
  _db.query_executed();
  return ;
}

/**
 *  Retrieve the next record in the result set.
 *
 *  @return True if there a record could be returned.
 */
bool database_query::next() {
  return (_q.next());
}

/**
 *  Prepare a statement.
 *
 *  @param[in] query      Statement.
 *  @param[in] error_msg  Error message in case of failure.
 */
void database_query::prepare(
                       std::string const& query,
                       char const* error_msg) {
  if (!_q.prepare(query.c_str())) {
    exceptions::msg e;
    if (error_msg)
      e << error_msg << ": ";
    e << "could not prepare query: " << _q.lastError().text();
    throw (e);
  }
  return ;
}

/**
 *  Returns the value at index of the record.
 *
 *  @param[in] index  Index in the record.
 *
 *  @return Value requested.
 */
QVariant database_query::value(int index) {
  return (_q.value(index));
}

/**
 *  @brief Copy constructor.
 *
 *  This method will abort the program.
 *
 *  @param[in] other  Unused.
 */
database_query::database_query(database_query const& other)
  : _db(other._db) {
  assert(!"database query objects are not copyable");
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  This method will abort the program.
 *
 *  @param[in] other  Unused.
 *
 *  @return This object.
 */
database_query& database_query::operator=(database_query const& other) {
  (void)other;
  assert(!"database query objects are not copyable");
  abort();
  return (*this);
}
