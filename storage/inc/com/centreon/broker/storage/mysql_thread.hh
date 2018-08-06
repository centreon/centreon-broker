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

#ifndef CCB_STORAGE_MYSQL_THREAD_HH
#  define CCB_STORAGE_MYSQL_THREAD_HH

#include <QThread>
#include <mysql.h>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace               storage {
  /**
   *  @class mysql_thread mysql_thread.hh "com/centreon/broker/storage/mysql_thread.hh"
   *  @brief Class representing a thread connected to the mysql server
   *
   */
  class                 mysql_thread : public QThread {
   public:
                        mysql_thread(
                          std::string const& address,
                          std::string const& user,
                          std::string const& password,
                          std::string const& database,
                          int port);
                        ~mysql_thread();

   private:
    void                run();

    MYSQL*              _conn;
  };
}

CCB_END()

#  endif  //CCB_STORAGE_MYSQL_THREAD_HH
