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

#ifndef CCB_MYSQL_ERROR_HH
#define CCB_MYSQL_ERROR_HH

#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace database {
/**
 *  @class mysql_thread mysql_thread.hh
 * "com/centreon/broker/storage/mysql_thread.hh"
 *  @brief Class representing a thread connected to the mysql server
 *
 */
class mysql_error {
 public:
  mysql_error();
  mysql_error(mysql_error&& other);
  mysql_error(char const* message, bool fatal);
  mysql_error& operator=(mysql_error const& other);
  bool is_fatal() const;
  std::string get_message() const;
  void clear();
  bool is_active() const;

 private:
  std::string _message;
  bool _active;
  bool _fatal;
};
}  // namespace database

CCB_END()

#endif  // CCB_MYSQL_ERROR_HH
