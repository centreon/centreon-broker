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

#ifndef CCB_IO_EVENT_INFO_HH
#define CCB_IO_EVENT_INFO_HH

#include <cstddef>
#include <string>

#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
namespace mapping {
class entry;
}

namespace io {
// Forward declaration.
class data;

/**
 *  @class event_info event_info.hh "com/centreon/broker/io/event_info.hh"
 *  @brief Provide information about an event.
 *
 *  This class is mostly used to provide information about a mapped
 *  type. Various event manipulation engines (BBDO, SQL) use
 *  information provided by this class.
 */
class event_info {
 public:
  struct event_operations {
    io::data* (*constructor)();
  };

 private:
  const mapping::entry* const _mapping;
  const std::string _name;
  const event_operations* _ops;
  const std::string _table_v2;

 public:
  /**
   *  Constructor.
   *
   *  @param[in] name      Event name.
   *  @param[in] ops       Event operations (constructor, ...).
   *  @param[in] entries   Event property mapping.
   *  @param[in] table_v2  SQL table of event in version 2.x (if any).
   */
  event_info(std::string const& name,
             event_operations const* ops,
             mapping::entry const* entries,
             std::string const& table_v2)
      : _mapping(entries), _name(name), _ops(ops), _table_v2(table_v2) {}
  /**
   *  Copy constructor.
   *
   *  @param[in] other  Object to copy.
   */
  event_info(event_info const& other)
      : _mapping(other._mapping),
        _name(other._name),
        _ops(other._ops),
        _table_v2(other._table_v2) {}
  ~event_info() = default;
  event_info& operator=(event_info const&) = delete;
  const mapping::entry* get_mapping() const { return _mapping; }
  const std::string& get_name() const { return _name; }
  const event_operations& get_operations() const { return *_ops; }
  const std::string& get_table_v2() const { return _table_v2; }
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_EVENT_INFO_HH
