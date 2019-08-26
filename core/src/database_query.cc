/*
** Copyright 2014-2015,2017 Centreon
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

#include <cstdlib>
#include <QSqlError>
#include "com/centreon/broker/database.hh"
#include "com/centreon/broker/database_query.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Get a boolean from an object.
 */
static void bind_boolean(
              QString const& field,
              bool val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get a double from an object.
 */
static void bind_double(
              QString const& field,
              double val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an integer from an object.
 */
static void bind_integer(
              QString const& field,
              int val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an integer that is null on zero.
 */
static void bind_integer_null_on_zero(
              QString const& field,
              int val,
              database_query& q) {
  // Not-NULL.
  if (val)
    q.bind_value(field, QVariant(val));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an integer that is null on -1.
 */
static void bind_integer_null_on_minus_one(
              QString const& field,
              int val,
              database_query& q) {
  // Not-NULL.
  if (val != -1)
    q.bind_value(field, QVariant(val));
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get a short from an object.
 */
static void bind_short(
              QString const& field,
              short val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
}

/**
 *  Get a string from an object.
 */
static void bind_string(
              QString const& field,
              QString const& val,
              database_query& q) {
  q.bind_value(field, QVariant(val.toStdString().c_str()));
}

/**
 *  Get a string that might be null from an object.
 */
static void bind_string_null_on_empty(
              QString const& field,
              QString const& val,
              database_query& q) {
  // Not-NULL.
  if (!val.isEmpty())
    q.bind_value(field, QVariant(val.toStdString().c_str()));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::String));
}

/**
 *  Get a time_t from an object.
 */
static void bind_timet(
              QString const& field,
              time_t val,
              database_query& q) {
  q.bind_value(
      field,
      QVariant(static_cast<qlonglong>(val)));
  return ;
}

/**
 *  Get a time_t that is null on 0.
 */
static void bind_timet_null_on_zero(
              QString const& field,
              time_t val,
              database_query& q) {
  qlonglong vall(val);
  // Not-NULL.
  if (vall)
    q.bind_value(field, QVariant(vall));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::LongLong));
  return ;
}

/**
 *  Get a time_t that is null on -1.
 */
static void bind_timet_null_on_minus_one(
              QString const& field,
              time_t val,
              database_query& q) {
  qlonglong vall(val);
  // Not-NULL.
  if (vall != -1)
    q.bind_value(field, QVariant(vall));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::LongLong));
  return ;
}

/**
 *  Get an unsigned int from an object.
 */
static void bind_uint(
              QString const& field,
              unsigned int val,
              database_query& q) {
  q.bind_value(field, QVariant(val));
  return ;
}

/**
 *  Get an unsigned int that is null on zero.
 */
static void bind_uint_null_on_zero(
              QString const& field,
              unsigned int val,
              database_query& q) {
  // Not-NULL.
  if (val)
    q.bind_value(field, QVariant(val));
  // NULL
  else
    q.bind_value(field, QVariant(QVariant::Int));
  return ;
}

/**
 *  Get an unsigned int that is null on -1.
 */
static void bind_uint_null_on_minus_one(
              QString const& field,
              unsigned int val,
              database_query& q) {
  // Not-NULL.
  if (val != (unsigned int)-1)
    q.bind_value(field, QVariant(val));
  // NULL.
  else
    q.bind_value(field, QVariant(QVariant::Int));
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
 *  @param[in] db  Database object.
 */
database_query::database_query(database& db)
  : _db(db), _q(db.get_qt_db()), _prepared(false) {
  _q.setForwardOnly(true);
}

/**
 *  Destructor.
 */
database_query::~database_query() {}

/**
 *  Extract data from object to DB row.
 *
 *  @param[out] q  Query object.
 *  @param[in]  d  Object.
 */
database_query& database_query::operator<<(io::data const& d) {
  // Get event info.
  io::event_info const*
    info(io::events::instance().get_event_info(d.type()));
  if (info) {
    bool db_v2(_db.schema_version() == database::v2);
    for (mapping::entry const* current_entry(info->get_mapping());
         !current_entry->is_null();
         ++current_entry) {
      char const* entry_name;
      if (db_v2)
        entry_name = current_entry->get_name_v2();
      else
        entry_name = current_entry->get_name();
      if (entry_name
          && entry_name[0]
          && (_excluded.find(entry_name) == _excluded.end())) {
        QString field(":");
        field.append(entry_name);
        switch (current_entry->get_type()) {
        case mapping::source::BOOL:
          bind_boolean(field, current_entry->get_bool(d), *this);
          break ;
        case mapping::source::DOUBLE:
          bind_double(field, current_entry->get_double(d), *this);
          break ;
        case mapping::source::INT:
          switch (current_entry->get_attribute()) {
          case mapping::entry::invalid_on_zero:
            bind_integer_null_on_zero(
              field,
              current_entry->get_int(d),
              *this);
            break ;
          case mapping::entry::invalid_on_minus_one:
            bind_integer_null_on_minus_one(
              field,
              current_entry->get_int(d),
              *this);
            break ;
          default:
            bind_integer(field, current_entry->get_int(d), *this);
          }
          break ;
        case mapping::source::SHORT:
          bind_short(field, current_entry->get_short(d), *this);
          break ;
        case mapping::source::STRING:
          if (current_entry->get_attribute()
              == mapping::entry::invalid_on_zero)
            bind_string_null_on_empty(
              field,
              current_entry->get_string(d).c_str(),
              *this);
          else
            bind_string(field, current_entry->get_string(d).c_str(), *this);
          break ;
        case mapping::source::TIME:
          switch (current_entry->get_attribute()) {
          case mapping::entry::invalid_on_zero:
            bind_timet_null_on_zero(
              field,
              current_entry->get_time(d),
              *this);
            break ;
          case mapping::entry::invalid_on_minus_one:
            bind_timet_null_on_minus_one(
              field,
              current_entry->get_time(d),
              *this);
            break ;
          default:
            bind_timet(field, current_entry->get_time(d), *this);
          }
          break ;
        case mapping::source::UINT:
          switch (current_entry->get_attribute()) {
          case mapping::entry::invalid_on_zero:
            bind_uint_null_on_zero(
              field,
              current_entry->get_uint(d),
              *this);
            break ;
          case mapping::entry::invalid_on_minus_one:
            bind_uint_null_on_minus_one(
              field,
              current_entry->get_uint(d),
              *this);
            break ;
          default :
            bind_uint(field, current_entry->get_uint(d), *this);
          }
          break ;
        default: // Error in one of the mappings.
          throw (exceptions::msg() << "invalid mapping for object "
                 << "of type '" << info->get_name() << "': "
                 << current_entry->get_type()
                 << " is not a known type ID");
        };
      }
    }
  }
  else
    throw (exceptions::msg() << "cannot bind object of type "
           << d.type() << " to database query: mapping does not exist");
  return (*this);
}

/**
 *  Set the placeholder to the value in the prepared statement.
 *
 *  @param[in] placeholder  Placeholder in the prepared statement.
 *  @param[in] value        Value.
 */
void database_query::bind_value(
                       QString const& placeholder,
                       QVariant const& value) {
  if (_doubled_fields.find(placeholder) == _doubled_fields.end())
    _q.bindValue(placeholder, value);
  else {
    _q.bindValue(placeholder + "1", value);
    _q.bindValue(placeholder + "2", value);
  }
  return ;
}

/**
 *  Get database object associated to this query.
 *
 *  @return Database object.
 */
database const& database_query::db_object() const {
  return (_db);
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
 *  Set excluded fields.
 *
 *  @param[in] excluded  Excluded fields.
 */
void database_query::set_excluded(
                       database_query::excluded_fields const& excluded) {
  _excluded = excluded;
  return ;
}

/**
 *  Set doubled fields.
 *
 *  @param[in] doubled  Doubled fields.
 */
void database_query::set_doubled(doubled_fields const& doubled) {
  _doubled_fields = doubled;
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
    _db.set_error();
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
    _db.set_error();
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
  logging::debug(logging::medium)
    << "core: preparing query: " << query;
  if (!_q.prepare(query.c_str())) {
    _db.set_error();
    exceptions::msg e;
    if (error_msg)
      e << error_msg << ": ";
    e << "could not prepare query: " << _q.lastError().text();
    throw (e);
  }
  _prepared = true;
  return ;
}

/**
 *  Check if query has been prepared.
 *
 *  @return True if query has been prepared.
 */
bool database_query::prepared() const {
  return (_prepared);
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
