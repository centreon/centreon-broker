/*
** Copyright 2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_DATABASE_HH
#  define CCB_DATABASE_HH

#  include <memory>
#  include <QSqlDatabase>
#  include <string>
#  include "com/centreon/broker/database_config.hh"
#  include "com/centreon/broker/namespace.hh"

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

private:
                    database(database const& other);
  database&         operator=(database const& other);
  void              _commit();
  void              _new_transaction();

  std::auto_ptr<QSqlDatabase>
                    _db;
  QString           _connection_id;
  database_config   _db_cfg;
  int               _pending_queries;
  bool              _committed;
};

CCB_END()

#endif // !CCB_DATABASE_HH
