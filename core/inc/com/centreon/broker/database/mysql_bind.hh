/*
** Copyright 2018 Centreon
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

#ifndef CCB_MYSQL_BIND_HH
#define CCB_MYSQL_BIND_HH

#include <mysql.h>
#include <string>
#include <vector>
#include "com/centreon/broker/database/mysql_column.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations
class mysql;

namespace database {
class mysql_bind {
 public:
  mysql_bind();
  mysql_bind(int size, int length = 0);
  ~mysql_bind();
  void set_size(int size, int length = 0);
  int value_as_i32(int range) const;
  void set_value_as_i32(int range, int value);
  uint32_t value_as_u32(int range) const;
  void set_value_as_u32(int range, uint32_t value);
  unsigned long long value_as_u64(int range) const;
  void set_value_as_u64(int range, unsigned long long value);
  bool value_as_bool(int range) const;
  void set_value_as_bool(int range, bool value);
  float value_as_f32(int range) const;
  void set_value_as_f32(int range, float value);
  double value_as_f64(int range) const;
  void set_value_as_f64(int range, double value);
  void set_value_as_null(int range);
  void set_value_as_tiny(int range, char value);
  char* value_as_str(int range);
  void set_value_as_str(int range, std::string const& value);
  int get_size() const;
  bool value_is_null(int range) const;
  bool is_empty() const;
  void set_empty(bool empty);
  int get_rows_count() const;

  MYSQL_BIND const* get_bind() const;
  MYSQL_BIND* get_bind();

  void debug();

 private:
  bool _prepared(int range) const;
  void _prepare_type(int range, enum enum_field_types type);

  std::vector<MYSQL_BIND> _bind;

  // The buffers contained by _bind
  std::vector<database::mysql_column> _column;

  // A vector telling if bindings are already typed or not.
  std::vector<bool> _typed;

  bool _is_empty;
};
}  // namespace database

CCB_END()

#endif  // CCB_MYSQL_BIND_HH
