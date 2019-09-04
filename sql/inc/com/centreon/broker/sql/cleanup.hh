/*
** Copyright 2014 Centreon
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

#ifndef CCB_SQL_CLEANUP_HH
#define CCB_SQL_CLEANUP_HH

#include <mutex>
#include <thread>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace sql {
/**
 *  @class cleanup cleanup.hh "com/centreon/broker/sql/cleanup.hh"
 *  @brief Check to cleanup database.
 *
 *  Check to cleanup database.
 */
class cleanup {
 public:
  cleanup(std::string const& db_type,
          std::string const& db_host,
          unsigned short db_port,
          std::string const& db_user,
          std::string const& db_password,
          std::string const& db_name,
          uint32_t cleanup_interval = 600);
  ~cleanup() throw();
  void exit() throw();
  uint32_t get_interval() const throw();
  void _run();
  void start();

 private:
  cleanup(cleanup const& other);
  cleanup& operator=(cleanup const& other);

  bool should_exit() const;

  std::thread _thread;
  std::string _db_type;
  std::string _db_host;
  unsigned short _db_port;
  std::string _db_user;
  std::string _db_password;
  std::string _db_name;
  const uint32_t _interval;

  mutable std::mutex _start_stop_m;
  bool _started;
  bool _should_exit;
};
}  // namespace sql

CCB_END()

#endif  // !CCB_SQL_CLEANUP_HH
