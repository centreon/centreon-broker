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

#include "test/table_content.hh"
#include <algorithm>

using namespace com::centreon::broker;
using namespace com::centreon::broker::test;

/**
 *  Default constructor.
 */
table_content::table_content() : _columns(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
table_content::table_content(table_content const& other)
    : _columns(other._columns), _content(other._content), _order_by(NULL) {}

/**
 *  Destructor.
 */
table_content::~table_content() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
table_content& table_content::operator=(table_content const& other) {
  if (this != &other) {
    _columns = other._columns;
    _content = other._content;
  }
  return (*this);
}

/**
 *  Get modifiable table row.
 *
 *  @param[in] index  Index of the row, starting at 0.
 */
std::vector<predicate>& table_content::operator[](int index) {
  return (_content[index]);
}

/**
 *  Get non-modifiable table row.
 *
 *  @param[in] index  Index of the row, starting at 0.
 */
std::vector<predicate> const& table_content::operator[](int index) const {
  return (_content[index]);
}

/**
 *  Get column number.
 *
 *  @return Number of columns in this table content.
 */
int table_content::get_columns() const {
  return (_columns);
}

/**
 *  Get rows number.
 *
 *  @return Number of rows in this table content.
 */
int table_content::get_rows() const {
  return (_content.size());
}

/**
 *  Insert a row in table content.
 *
 *  @param[in] row  Array of columns() elements.
 */
void table_content::insert_row(predicate const* row) {
  int index(_content.size());
  _content.resize(index + 1);
  _content[index].resize(_columns);
  for (int i(0); i < _columns; ++i)
    _content[index][i] = row[i];
  sort();
  return;
}

/**
 *  Set order by function.
 *
 *  @param[in] func  Order by function.
 */
void table_content::set_order_by(table_content::order_by func) {
  _order_by = func;
  sort();
  return;
}

/**
 *  Sort table content.
 */
void table_content::sort() {
  if (_order_by)
    std::sort(_content.begin(), _content.end(), *_order_by);
  return;
}
