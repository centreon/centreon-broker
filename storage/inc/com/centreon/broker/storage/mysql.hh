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
#ifndef CCB_STORAGE_MYSQL_HH
#  define CCB_STORAGE_MYSQL_HH

#  include "com/centreon/broker/storage/mysql_bind.hh"
#  include "com/centreon/broker/storage/mysql_thread.hh"

CCB_BEGIN()

namespace           storage {
  /**
   *  @class mysql mysql.hh "com/centreon/broker/storage/mysql.hh"
   *  @brief Class managing the mysql connection
   *
   *  Here is a binding to the C MySQL connector.
   */
  class                 mysql {
   public:
    enum                version {
      v2 = 2,
      v3
    };
                        mysql(database_config const& db_cfg);
                        ~mysql();
    int                 prepare_query(std::string const& query);
    void                commit();
    void                run_query(
                          std::string const& query,
                          mysql_callback fn = 0, void* data = 0,
                          std::string const& error_msg = "", bool fatal = false,
                          int thread = -1);
    int                 run_query_sync(
                          std::string const& query,
                          std::string const& error_msg = "",
                          int thread = -1);
    void                run_statement(
                          int statement_id, mysql_bind const& bind,
                          mysql_callback fn = 0, void* data = 0,
                          std::string const& error_msg = "", bool fatal = false,
                          int thread = -1);
    mysql_result        get_result(int thread_id);
    bool                finish();
    version             schema_version() const;
    void                register_commit_callback(mysql_callback fn, void* data);

   private:
    void                _commit_if_needed();
    void                _check_errors(int thread_id);

    database_config     _db_cfg;
    int                 _pending_queries;

    std::vector<mysql_thread*>
                        _vector;
    version             _version;
    std::vector<mysql_thread*>
                        _thread;
    int                 _current_thread;
    int                 _prepare_count;
    mysql_callback      _commit_callback;
    void*               _commit_data;
  };
}

CCB_END()

#  endif  //CCB_STORAGE_MYSQL_HH
