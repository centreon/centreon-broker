/*
** Copyright 2015 Merethis
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
#include "com/centreon/broker/database_preparator.hh"
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
database_preparator::database_preparator(
                       unsigned int event_id,
                       database_preparator::event_unique const& unique,
                       database_query::excluded_fields const& excluded)
  : _event_id(event_id), _excluded(excluded), _unique(unique) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
database_preparator::database_preparator(
                       database_preparator const& other) {
  _event_id = other._event_id;
  _excluded = other._excluded;
  _unique = other._unique;
}

/**
 *  Destructor.
 */
database_preparator::~database_preparator() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
database_preparator& database_preparator::operator=(
                       database_preparator const& other) {
  if (this != &other) {
    _event_id = other._event_id;
    _excluded = other._excluded;
    _unique = other._unique;
  }
  return (*this);
}

/**
 *  Prepare insertion query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
void database_preparator::prepare_insert(database_query& q) {
  // Find event info.
  io::event_info const*
    info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw (exceptions::msg()
           << "could not prepare insertion query for event of type "
           << _event_id << ": event is not registered");

  // Build query string.
  std::string query;
  query = "INSERT INTO ";
  query.append(info->get_table());
  query.append(" (");
  mapping::entry const* entries(info->get_mapping());
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name(entries[i].get_name());
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
    char const* entry_name(entries[i].get_name());
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
    q.prepare(query);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "could not prepare insertion query for event '"
           << info->get_name() << "' in table '"
           << info->get_table() << "': " << e.what());
  }

  return ;
}

/**
 *  Prepare update query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
void database_preparator::prepare_update(database_query& q) {
  // Find event info.
  io::event_info const*
    info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw (exceptions::msg()
           << "could not prepare update query for event of type "
           << _event_id << ": event is not registered");

  // Build query string.
  std::string query;
  std::string where;
  query = "UPDATE ";
  query.append(info->get_table());
  query.append(" SET ");
  where = " WHERE ";
  mapping::entry const* entries(info->get_mapping());
  for (int i(0); !entries[i].is_null(); ++i) {
    char const* entry_name(entries[i].get_name());
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
      where.append("COALESCE(");
      where.append(entry_name);
      where.append(", -1)=COALESCE(:");
      where.append(entry_name);
      where.append(", -1) AND ");
    }
  }
  query.resize(query.size() - 2);
  query.append(where, 0, where.size() - 5);

  // Prepare statement.
  try {
    q.prepare(query);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "could not prepare update query for event '"
           << info->get_name() << "' on table '"
           << info->get_table() << "': " << e.what());
  }

  return ;
}

/**
 *  Prepare deletion query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
void database_preparator::prepare_delete(database_query& q) {
  // Find event info.
  io::event_info const*
    info(io::events::instance().get_event_info(_event_id));
  if (!info)
    throw (exceptions::msg()
           << "could not prepare deletion query for event of type "
           << _event_id << ": event is not registered");

  // Prepare query.
  std::string query;
  query = "DELETE FROM ";
  query.append(info->get_table());
  query.append(" WHERE ");
  for (event_unique::const_iterator
         it(_unique.begin()),
         end(_unique.end());
       it != end;
       ++it) {
    query.append("COALESCE(");
    query.append(*it);
    query.append(", -1)=COALESCE(:");
    query.append(*it);
    query.append(", -1) AND ");
  }
  query.resize(query.size() - 5);

  // Prepare statement.
  try {
    q.prepare(query);
  }
  catch (std::exception const& e) {
    throw (exceptions::msg()
           << "could not prepare deletion query for event '"
           << info->get_name() << "' on table '"
           << info->get_table() << "': " << e.what());
  }

  return ;
}
