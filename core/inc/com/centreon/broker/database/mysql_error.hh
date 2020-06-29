/*
** Copyright 2018-2020 Centreon
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

#include <atomic>
#include <string>
#include <fmt/format.h>
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
  mysql_error(): _active(false) {}
  mysql_error(mysql_error const& other) = delete;
  mysql_error(mysql_error&& other) = delete;
  mysql_error(char const* message) : _message(message), _active(true) {}
  mysql_error& operator=(mysql_error const& other) = delete;
  std::string get_message() { return std::move(_message); }

  template<typename... Args>
  void set_message(std::string const& format, const Args&... args) {
    _message = fmt::format(format, args...);
    _active = true;
  }
  void clear() { _active = false; }
  bool is_active() const { return _active; }

 private:
  std::string _message;
  std::atomic<bool> _active;
};
}  // namespace database

CCB_END()

#endif  // CCB_MYSQL_ERROR_HH
