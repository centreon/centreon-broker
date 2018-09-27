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

#ifndef CCB_MYSQL_STMT_HH
#  define CCB_MYSQL_STMT_HH

#include <string>
#include <map>
#include <mysql.h>
#include "com/centreon/broker/misc/shared_ptr.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration
class                           mysql_bind;

typedef                         std::map<std::string, int> mysql_stmt_mapping;

class                           mysql_stmt {
 public:
                                mysql_stmt();
                                mysql_stmt(mysql_stmt const& other);
                                ~mysql_stmt();
                                mysql_stmt(
                                  MYSQL* conn,
                                  std::string const& query,
                                  mysql_stmt_mapping const& mapping = mysql_stmt_mapping());
  mysql_stmt&                   operator=(mysql_stmt const& other);
  int                           bind(mysql_bind const& bind);
  int                           execute();
  char const*                   get_error();
  mysql_stmt_mapping            get_mapping() const;

 private:
  misc::shared_ptr<MYSQL_STMT>  _stmt;
  mysql_stmt_mapping            _bind_mapping;
};

CCB_END()

#endif // CCB_MYSQL_STMT_HH
