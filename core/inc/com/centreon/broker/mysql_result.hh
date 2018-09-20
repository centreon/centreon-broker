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
#ifndef CCB_MYSQL_RESULT_HH
#  define CCB_MYSQL_RESULT_HH

#include <mysql.h>
#include <string>
#include "com/centreon/broker/misc/shared_ptr.hh"

CCB_BEGIN()

/**
 *  @class mysql mysql.hh "com/centreon/broker/storage/mysql.hh"
 *  @brief Class managing the mysql connection
 *
 *  Here is a binding to the C MYSQL_RES. This is useful because of the
 *  delete functionality that must call the mysql_free_result() function.
 */
class                 mysql_result {
 public:
                      mysql_result(MYSQL_RES* res = NULL);
  mysql_result&       operator=(mysql_result const& other);
  bool                next();
  bool                value_as_bool(int idx);
  double              value_as_f64(int idx);
  int                 value_as_i32(int idx);
  std::string         value_as_str(int idx);
  unsigned int        value_as_u32(int idx);
  unsigned long long  value_as_u64(int idx);
  bool                value_is_null(int idx);
  bool                is_empty() const;
  int                 get_rows_count() const;

 private:
  misc::shared_ptr<MYSQL_RES>
                      _result;
  MYSQL_ROW           _row;
};

CCB_END()

#  endif  //CCB_MYSQL_RESULT_HH
