/*
** Copyright 2015 Centreon
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

#include <sstream>
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/broker/query_preparator.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"

using namespace com::centreon::broker;

/**
 *  Constructor.
 *
 *  @param[in] event_id  Event ID.
 *  @param[in] unique    Event UNIQUE.
 *  @param[in] excluded  Fields excluded from the query.
 */
query_preparator::query_preparator(
                       unsigned int event_id,
                       query_preparator::event_unique const& unique,
                       query_preparator::excluded_fields const& excluded)
  : _event_id(event_id), _excluded(excluded), _unique(unique) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
query_preparator::query_preparator(
                       query_preparator const& other) {
  _event_id = other._event_id;
  _excluded = other._excluded;
  _unique = other._unique;
}

/**
 *  Destructor.
 */
query_preparator::~query_preparator() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
query_preparator& query_preparator::operator=(
                       query_preparator const& other) {
  if (this != &other) {
    _event_id = other._event_id;
    _excluded = other._excluded;
    _unique = other._unique;
  }
  return *this;
}

/**
 *  Prepare insertion query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
int query_preparator::prepare_insert(mysql& ms) {
  int retval;
  // Find event info.
  io::event_info const*
    info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw (exceptions::msg()
           << "could not prepare insertion query for event of type "
           << _event_id << ": event is not registered");

  // Database schema version.
  bool schema_v2(ms.schema_version() == mysql::v2);

  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  if (schema_v2)
    query.append(info->get_table_v2());
  else
    query.append(info->get_table());
  query.append(" (");
  mapping::entry const* entries(info->get_mapping());
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    if (schema_v2)
      entry_name = entries[i].get_name_v2();
    else
      entry_name = entries[i].get_name();
    if (!entry_name
        || !entry_name[0]
        || (_excluded.find(entry_name) != _excluded.end()))
      continue ;
    query.append(entry_name);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    if (schema_v2)
      entry_name = entries[i].get_name_v2();
    else
      entry_name = entries[i].get_name();
    if (!entry_name
        || !entry_name[0]
        || (_excluded.find(entry_name) != _excluded.end()))
      continue ;
    query.append(":");
    query.append(entry_name);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(")");

  // Prepare statement.
  try {
    retval = ms.prepare_query(query);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "could not prepare insertion query for event '"
           << info->get_name() << "' in table '"
           << info->get_table() << "': " << e.what());
  }
  return retval;
}

/**
 *  Prepare update query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
int query_preparator::prepare_update(mysql& ms) {
  int retval;
  // Find event info.
  io::event_info const*
    info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw (exceptions::msg()
           << "could not prepare update query for event of type "
           << _event_id << ": event is not registered");

  // Database schema version.
  bool schema_v2(ms.schema_version() == mysql::v2);

  // Build query string.
  std::string query;
  std::string where;
  query = "UPDATE ";
  if (schema_v2)
    query.append(info->get_table_v2());
  else
    query.append(info->get_table());
  query.append(" SET ");
  where = " WHERE ";
  mapping::entry const* entries(info->get_mapping());
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    if (schema_v2)
      entry_name = entries[i].get_name_v2();
    else
      entry_name = entries[i].get_name();
    if (!entry_name
        || !entry_name[0]
        || (_excluded.find(entry_name) != _excluded.end()))
      continue ;
    // Standard field.
    if (_unique.find(entry_name) == _unique.end()) {
      query.append(entry_name);
      query.append("=:");
      query.append(entry_name);
      query.append(", ");
    }
    // Part of ID field.
    else {
      //FIXME DBR Here, I don't know how to do things since we have two
      //possible values to manage.
      where.append("((");
      where.append(entry_name);
      where.append("=:");
      where.append(entry_name);
      where.append("1) OR (");
      where.append(entry_name);
      where.append(" IS NULL AND :");
      where.append(entry_name);
      where.append("2 IS NULL)) AND ");
    }
  }
  query.resize(query.size() - 2);
  query.append(where, 0, where.size() - 5);

  //FIXME DBR: idem here
  // Get doubled fields.
  query_preparator::doubled_fields doubled;
  for (event_unique::const_iterator
         it = _unique.begin(),
         end = _unique.end();
       it != end;
       ++it)
    doubled.insert(QString(":") + QString::fromStdString(*it));
  // FIXME DBR: What to do with that ?
  //q.set_doubled(doubled);

  // Prepare statement.
  try {
    retval = ms.prepare_query(query);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "could not prepare update query for event '"
           << info->get_name() << "' on table '"
           << info->get_table() << "': " << e.what());
  }
  return retval;
}

/**
 *  Prepare deletion query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
int query_preparator::prepare_delete(mysql& ms) {
  int retval;
  // Find event info.
  io::event_info const*
    info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw (exceptions::msg()
           << "could not prepare deletion query for event of type "
           << _event_id << ": event is not registered");

  // Database schema version.
  bool schema_v2(ms.schema_version() == mysql::v2);

  // Prepare query.
  std::string query;
  query = "DELETE FROM ";
  if (schema_v2)
    query.append(info->get_table_v2());
  else
    query.append(info->get_table());
  query.append(" WHERE ");
  for (event_unique::const_iterator
         it(_unique.begin()),
         end(_unique.end());
       it != end;
       ++it) {
    query.append("((");
    query.append(*it);
    query.append("=:");
    query.append(*it);
    query.append("1) OR (");
    query.append(*it);
    query.append(" IS NULL AND :");
    query.append(*it);
    query.append("2 IS NULL)) AND ");
  }
  query.resize(query.size() - 5);

  // Get doubled fields.
  query_preparator::doubled_fields doubled;
  for (event_unique::const_iterator
         it = _unique.begin(),
         end = _unique.end();
       it != end;
       ++it)
    doubled.insert(QString(":") + QString::fromStdString(*it));
  // FIXME DBR: What to do with that ?
  //q.set_doubled(doubled);

  // Prepare statement.
  try {
    retval = ms.prepare_query(query);
  }
  catch (std::exception const& e) {
    //FIXME DBR
    throw (exceptions::msg()
           << "could not prepare deletion query for event '"
           << info->get_name() << "' on table '"
           << info->get_table() << "': " << e.what());
  }
  return retval;
}
