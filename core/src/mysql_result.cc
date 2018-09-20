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
#include <cstdlib>
#include "com/centreon/broker/mysql_result.hh"

using namespace com::centreon::broker;

mysql_result::mysql_result(MYSQL_RES* result)
  : _result(result, mysql_free_result) {}

mysql_result& mysql_result::operator=(mysql_result const& other) {
  _result = other._result;
  return *this;
}

bool mysql_result::next() {
  _row = mysql_fetch_row(_result.data());
  return _row != 0;
}

bool mysql_result::value_as_bool(int idx) {
  bool retval(_row[idx] ? strtol(_row[idx], 0, 10) : 0);
  return retval;
}

std::string mysql_result::value_as_str(int idx) {
  std::string retval(_row[idx]);
  return retval;
}

double mysql_result::value_as_f64(int idx) {
  double retval(_row[idx] ? atof(_row[idx]) : 0);
  return retval;
}

int mysql_result::value_as_i32(int idx) {
  int retval(_row[idx] ? strtol(_row[idx], 0, 10) : 0);
  return retval;
}

unsigned int mysql_result::value_as_u32(int idx) {
  unsigned int retval(_row[idx] ? strtoul(_row[idx], 0, 10) : 0);
  return retval;
}

unsigned long long mysql_result::value_as_u64(int idx) {
  unsigned long long retval(_row[idx] ? strtoull(_row[idx], 0, 10) : 0);
  return retval;
}

bool mysql_result::value_is_null(int idx) {
  return _row[idx] == 0;
}

bool mysql_result::is_empty() const {
  return _result.data() == NULL;
}

int mysql_result::get_rows_count() const {
  return mysql_num_rows(_result.data());
}
