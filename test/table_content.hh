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

#ifndef CCB_TEST_TABLE_CONTENT_HH
#define CCB_TEST_TABLE_CONTENT_HH

#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "test/predicate.hh"

CCB_BEGIN()

namespace test {
/**
 *  @class table_content table_content.hh "test/table_content.hh"
 *  @brief Expected content of a table.
 *
 *  Expected content of a table.
 */
class table_content {
 public:
  typedef bool (*order_by)(std::vector<predicate> const&,
                           std::vector<predicate> const&);
  table_content();
  table_content(table_content const& other);
  template <int columns>
  table_content(predicate const content[][columns]) : _columns(columns) {
    for (int i(0); content[i][0].is_valid(); ++i) {
      _content.resize(i + 1);
      _content[i].resize(columns);
      for (int j(0); j < columns; ++j)
        _content[i][j] = content[i][j];
    }
  }
  ~table_content();
  table_content& operator=(table_content const& other);
  std::vector<predicate>& operator[](int index);
  std::vector<predicate> const& operator[](int index) const;
  int get_columns() const;
  int get_rows() const;
  void insert_row(predicate const* row);
  void set_order_by(order_by func = NULL);
  void sort();

 private:
  int _columns;
  std::vector<std::vector<predicate> > _content;
  order_by _order_by;
};
}  // namespace test

CCB_END()

#endif  // !CCB_TEST_TABLE_CONTENT_HH
