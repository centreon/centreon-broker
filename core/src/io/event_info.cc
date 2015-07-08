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

#include "com/centreon/broker/io/event_info.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

/**
 *  Constructor.
 *
 *  @param[in] name     Event name.
 *  @param[in] ops      Event operations (constructor, ...).
 *  @param[in] entries  Event property mapping.
 *  @param[in] table    SQL table of event.
 */
event_info::event_info(
              std::string const& name,
              event_info::event_operations const* ops,
              mapping::entry const* entries,
              std::string const& table)
  : _mapping(entries), _name(name), _ops(ops), _table(table) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
event_info::event_info(event_info const& other)
  : _mapping(other._mapping),
    _name(other._name),
    _ops(other._ops),
    _table(other._table) {}

/**
 *  Destructor.
 */
event_info::~event_info() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
event_info& event_info::operator=(event_info const& other) {
  _mapping = other._mapping;
  _name = other._name;
  _ops = other._ops;
  _table = other._table;
  return (*this);
}

/**
 *  Get mapping.
 *
 *  @return Event mapping.
 */
mapping::entry const* event_info::get_mapping() const {
  return (_mapping);
}

/**
 *  Get event name.
 *
 *  @return Event name.
 */
std::string const& event_info::get_name() const {
  return (_name);
}

/**
 *  Get event operations.
 *
 *  @return Event operations.
 */
event_info::event_operations const& event_info::get_operations() const {
  return (*_ops);
}

/**
 *  Get event table.
 *
 *  @return Event SQL table name.
 */
std::string const& event_info::get_table() const {
  return (_table);
}
