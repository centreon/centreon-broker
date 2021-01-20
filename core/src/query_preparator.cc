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

#include "com/centreon/broker/query_preparator.hh"

#include <sstream>

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/mysql.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::database;

/**
 *  Constructor.
 *
 *  @param[in] event_id  Event ID.
 *  @param[in] unique    Event UNIQUE.
 *  @param[in] excluded  Fields excluded from the query.
 */
query_preparator::query_preparator(
    uint32_t event_id,
    query_preparator::event_unique const& unique,
    query_preparator::excluded_fields const& excluded)
    : _event_id(event_id), _excluded(excluded), _unique(unique) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
query_preparator::query_preparator(query_preparator const& other) {
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
query_preparator& query_preparator::operator=(query_preparator const& other) {
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
 *  @param[in] ignore  A boolean telling if the query ignores errors
 *                      (default value is false).
 */
mysql_stmt query_preparator::prepare_insert(mysql& ms, bool ignore) {
  std::map<std::string, int> bind_mapping;
  // Find event info.
  io::event_info const* info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw msg_fmt(
        "could not prepare insertion query for event of type {}: "
        "event is not registered",
        _event_id);

  // Build query string.
  std::string query;
  if (ignore)
    query = "INSERT IGNORE INTO ";
  else
    query = "INSERT INTO ";

  query.append(info->get_table_v2());
  query.append(" (");
  mapping::entry const* entries(info->get_mapping());
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    entry_name = entries[i].get_name_v2();
    if (!entry_name || !entry_name[0] ||
        (_excluded.find(entry_name) != _excluded.end()))
      continue;
    query.append(entry_name);
    query.append(", ");
  }
  query.resize(query.size() - 2);
  query.append(") VALUES(");
  std::string key;
  int size(0);
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    entry_name = entries[i].get_name_v2();
    if (!entry_name || !entry_name[0] ||
        (_excluded.find(entry_name) != _excluded.end()))
      continue;
    key = std::string(":");
    key.append(entry_name);
    bind_mapping.insert(std::make_pair(key, size++));
    query.append("?,");
  }
  query.resize(query.size() - 1);
  query.append(")");

  logging::debug(logging::low) << "mysql: query_preparator: " << query;
  // Prepare statement.
  mysql_stmt retval;
  try {
    retval = ms.prepare_query(query, bind_mapping);
  } catch (std::exception const& e) {
    throw msg_fmt(
        "could not prepare insertion query for event '{}' in table '{}': {}",
        info->get_name(), info->get_table_v2(), e.what());
  }
  return retval;
}

mysql_stmt query_preparator::prepare_insert_or_update(mysql& ms) {
  std::map<std::string, int> insert_bind_mapping;
  std::map<std::string, int> update_bind_mapping;
  // Find event info.
  io::event_info const* info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw msg_fmt(
        "could not prepare insertion query for event of type {} : "
        "event is not registered ",
        _event_id);

  // Build query string.
  std::string insert("INSERT INTO ");
  std::string update(" ON DUPLICATE KEY UPDATE ");
  insert.append(info->get_table_v2());
  insert.append(" (");
  mapping::entry const* entries(info->get_mapping());
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    entry_name = entries[i].get_name_v2();
    if (!entry_name || !entry_name[0] ||
        (_excluded.find(entry_name) != _excluded.end()))
      continue;
    insert.append(entry_name);
    insert.append(",");
  }
  insert.resize(insert.size() - 1);
  insert.append(") VALUES(");
  std::string key;
  int insert_size(0);
  int update_size(0);
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name;
    entry_name = entries[i].get_name_v2();
    if (!entry_name || !entry_name[0] ||
        (_excluded.find(entry_name) != _excluded.end()))
      continue;
    key = std::string(":");
    key.append(entry_name);
    if (_unique.find(entry_name) == _unique.end()) {
      update.append(entry_name);
      insert.append("?,");
      update.append("=?,");
      key.append("1");
      insert_bind_mapping.insert(std::make_pair(key, insert_size++));
      key[key.size() - 1] = '2';
      update_bind_mapping.insert(std::make_pair(key, update_size++));
    } else {
      insert.append("?,");
      insert_bind_mapping.insert(std::make_pair(key, insert_size++));
    }
  }
  insert.resize(insert.size() - 1);
  update.resize(update.size() - 1);
  insert.append(") ");
  insert.append(update);

  for (std::map<std::string, int>::const_iterator
           it(update_bind_mapping.begin()),
       end(update_bind_mapping.end());
       it != end; ++it)
    insert_bind_mapping.insert(
        std::make_pair(it->first, it->second + insert_size));

  logging::debug(logging::low) << "mysql: query_preparator: " << insert;
  // Prepare statement.
  mysql_stmt retval;
  try {
    retval = ms.prepare_query(insert, insert_bind_mapping);
  } catch (std::exception const& e) {
    throw msg_fmt(
        "could not prepare insert or update query for event '{}' in table "
        "'{}': {}",
        info->get_name(), info->get_table_v2(), e.what());
  }
  return retval;
}

/**
 *  Prepare update query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
mysql_stmt query_preparator::prepare_update(mysql& ms) {
  std::map<std::string, int> query_bind_mapping;
  std::map<std::string, int> where_bind_mapping;
  // Find event info.
  io::event_info const* info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw msg_fmt(
        "could not prepare update query for event of type {}:"
        "event is not registered",
        _event_id);

  // Build query string.
  std::string query("UPDATE ");
  std::string where(" WHERE ");
  query.append(info->get_table_v2());
  query.append(" SET ");
  mapping::entry const* entries(info->get_mapping());
  std::string key;
  int query_size(0);
  int where_size(0);
  for (int i = 0; !entries[i].is_null(); ++i) {
    char const* entry_name;
    entry_name = entries[i].get_name_v2();
    if (!entry_name || !entry_name[0] ||
        (_excluded.find(entry_name) != _excluded.end()))
      continue;
    // Standard field.
    if (_unique.find(entry_name) == _unique.end()) {
      query.append(entry_name);
      key = std::string(":");
      key.append(entry_name);
      query.append("=?,");
      query_bind_mapping.insert(std::make_pair(key, query_size++));
    }
    // Part of ID field.
    else {
      where.append(entry_name);
      where.append("=? AND ");
      key = std::string(":");
      key.append(entry_name);
      where_bind_mapping.insert(std::make_pair(key, where_size++));
    }
  }
  query.resize(query.size() - 1);
  query.append(where, 0, where.size() - 5);

  for (std::map<std::string, int>::iterator it(where_bind_mapping.begin()),
       end(where_bind_mapping.end());
       it != end; ++it)
    query_bind_mapping.insert(
        std::make_pair(it->first, it->second + query_size));

  // Prepare statement.
  mysql_stmt retval;
  try {
    retval = ms.prepare_query(query, query_bind_mapping);
  } catch (std::exception const& e) {
    throw msg_fmt(
        "could not prepare update query for event '{}': on table '{}': {}",
        info->get_name(), info->get_table_v2(), e.what());
  }
  return retval;
}

/**
 *  Prepare deletion query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
mysql_stmt query_preparator::prepare_delete(mysql& ms) {
  std::map<std::string, int> bind_mapping;
  // Find event info.
  io::event_info const* info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw msg_fmt(
        "could not prepare deletion query for event of type "
        " {}: event is not registered",
        _event_id);

  // Prepare query.
  std::string query("DELETE FROM ");
  query.append(info->get_table_v2());
  query.append(" WHERE ");
  int size = 0;
  for (event_unique::const_iterator it(_unique.begin()), end(_unique.end());
       it != end; ++it) {
    query.append("(");
    query.append(*it);
    query.append("=?");
    std::string key(":");
    key.append(*it);
    bind_mapping.insert(std::make_pair(key, size++));

    query.append(") AND ");
  }
  query.resize(query.size() - 5);

  // Prepare statement.
  mysql_stmt retval;
  try {
    retval = ms.prepare_query(query, bind_mapping);
  } catch (std::exception const& e) {
    // FIXME DBR
    throw msg_fmt(
        "could not prepare deletion query for event '{}' on table '{}': {}",
        info->get_name(), info->get_table_v2(), e.what());
  }
  return retval;
}
