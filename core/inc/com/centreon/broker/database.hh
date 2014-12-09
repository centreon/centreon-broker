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
                    database(
                      std::string const& type,
                      std::string const& host,
                      unsigned short port,
                      std::string const& user,
                      std::string const& password,
                      std::string const& db_name,
                      int queries_per_transaction = 1,
                      bool check_replication = false);
                    ~database();
  void              commit();
  QSqlDatabase&     get_qt_db();
  QSqlDriver const* get_qt_driver() const;
  int               pending_queries() const;
  static QString    qt_db_type(std::string const& broker_type);
  void              query_executed();

private:
                    database(database const& other);
  database&         operator=(database const& other);
  void              _commit();
  void              _new_transaction();

  QString           _connection_id;
  std::auto_ptr<QSqlDatabase>
                    _db;
  std::string       _db_name;
  std::string       _host;
  std::string       _password;
  int               _pending_queries;
  unsigned short    _port;
  int               _queries_per_transaction;
  std::string       _type;
  std::string       _user;
};

CCB_END()

#endif // !CCB_DATABASE_HH
