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
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/storage/mysql_thread.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

void mysql_thread::run() {
}

mysql_thread::mysql_thread(
                std::string const& address,
                std::string const& user,
                std::string const& password,
                std::string const& database,
                int port)
  : _conn(mysql_init(NULL)) {
  if (!_conn) {
    logging::error(logging::high)
      << "storage: Unable to establish MySQL server connection";
    return ;
  }

  if (!mysql_real_connect(
         _conn,
         address.c_str(),
         user.c_str(),
         password.c_str(),
         database.c_str(),
         port,
         NULL,
         0)) {
    logging::error(logging::high)
      << "storage: The connection to the MySQL database failed";
    return ;
  }
}

mysql_thread::~mysql_thread() {
  mysql_close(_conn);
  mysql_thread_end();
}
