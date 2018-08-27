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
#include "com/centreon/broker/storage/mysql_result.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

mysql_result::mysql_result(MYSQL_RES* result)
  : _result(result) {}

mysql_result::~mysql_result() {
  if (_result) {
    mysql_free_result(_result);
    _result = 0;
  }
}

bool mysql_result::is_empty() const {
  return _result == 0;
}

bool mysql_result::next() {
  _row = mysql_fetch_row(_result);
  return _row != 0;
}

int mysql_result::value(int idx) {
  int retval(atoi(_row[idx]));
  return retval;
}
