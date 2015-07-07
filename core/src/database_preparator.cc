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
  // XXX
}

/**
 *  Prepare update query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
void database_preparator::prepare_update(database_query& q) {
  // XXX
}

/**
 *  Prepare deletion query for specified event.
 *
 *  @param[out] q  Database query, prepared and ready to run.
 */
void database_preparator::prepare_delete(database_query& q) {
  // XXX
}
