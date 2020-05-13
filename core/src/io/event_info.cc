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

#include "com/centreon/broker/io/event_info.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::io;

/**
 *  Constructor.
 *
 *  @param[in] name      Event name.
 *  @param[in] ops       Event operations (constructor, ...).
 *  @param[in] entries   Event property mapping.
 *  @param[in] table     SQL table of event (if any).
 *  @param[in] table_v2  SQL table of event in version 2.x (if any).
 */
event_info::event_info(std::string const& name,
                       event_info::event_operations const* ops,
                       mapping::entry const* entries,
                       std::string const& table,
                       std::string const& table_v2)
    : _mapping(entries),
      _name(name),
      _ops(ops),
      _table(table),
      _table_v2(table_v2) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
event_info::event_info(event_info const& other)
    : _mapping(other._mapping),
      _name(other._name),
      _ops(other._ops),
      _table(other._table),
      _table_v2(other._table_v2) {}
