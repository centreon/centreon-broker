/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_INFLUXDB_COLUMN_HH
#define CCB_INFLUXDB_COLUMN_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace influxdb {
/**
 *  @class column column.hh "com/centreon/broker/influxdb/column.hh"
 *  @brief Store the data for a column in the query.
 */
class column {
 public:
  enum type { string, number };

  column();
  column(std::string const& name,
         std::string const& value,
         bool is_flag,
         type col_type);
  column(column const& c);
  column& operator=(column const& c);

  std::string const& get_name() const;
  std::string const& get_value() const;
  bool is_flag() const;
  type get_type() const;
  static type parse_type(std::string const& type);

 private:
  std::string _name;
  std::string _value;
  bool _is_flag;
  type _type;
};
}  // namespace influxdb

CCB_END()

#endif  // !CCB_INFLUXDB_COLUMN_HH
