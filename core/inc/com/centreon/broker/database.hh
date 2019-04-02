/*
** Copyright 2014-2015,2017 Centreon
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

#ifndef CCB_DATABASE_HH
#  define CCB_DATABASE_HH

#  include <memory>
#  include <QSqlDatabase>
#  include <string>
#  include "com/centreon/broker/database_config.hh"

CCB_BEGIN()

/**
 *  @class database database.hh "com/centreon/broker/database.hh"
 *  @brief Access databases used by Centreon Broker.
 *
 *  Access all databases accessed by Centreon Broker, namely *centreon*
 *  and *centreon_storage*.
 */
class               database {
 public:
  enum              version {
    v2 = 2,
    v3
  };

                    database(database_config const& db_cfg);
                    ~database();
  void              commit();
  QSqlDatabase&     get_qt_db();
  QSqlDriver const* get_qt_driver() const;
  int               pending_queries() const;
  bool              committed() const;
  void              clear_committed_flag();
  static QString    qt_db_type(std::string const& broker_type);
  void              query_executed();
  version           schema_version() const;
  void              set_error();

 private:
                    database(database const& other);
  database&         operator=(database const& other);
  void              _commit();
  void              _new_transaction();

  std::auto_ptr<QSqlDatabase>
                    _db;
  QString           _connection_id;
  database_config   _db_cfg;
  bool              _error;
  int               _pending_queries;
  bool              _committed;
  version           _version;
};

CCB_END()

#endif // !CCB_DATABASE_HH
