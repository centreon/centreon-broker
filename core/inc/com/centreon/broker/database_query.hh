/*
** Copyright 2014-2015 Merethis
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

#ifndef CCB_DATABASE_QUERY_HH
#  define CCB_DATABASE_QUERY_HH

#  include <QSqlQuery>
#  include <QString>
#  include <QVariant>
#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declaration.
class database;

/**
 *  @class database_query database_query.hh "com/centreon/broker/database_query.hh"
 *  @brief Database query.
 *
 *  Run a query on some Centreon Broker database.
 */
class               database_query {
public:
                    database_query(database& db);
                    ~database_query();
  database_query&   operator<<(io::data const& e);
  void              bind_value(
                      QString const& placeholder,
                      QVariant const& value);
  void              finish();
  QVariant          last_insert_id();
  int               num_rows_affected();
  int               size();
  void              run_query(
                      std::string const& query,
                      char const* error_msg = NULL);
  void              run_statement(char const* error_msg = NULL);
  bool              next();
  void              prepare(
                      std::string const& query,
                      char const* error_msg = NULL);
  QVariant          value(int index);

private:
                    database_query(database_query const& other);
  database_query&   operator=(database_query const& other);

  database&         _db;
  QSqlQuery         _q;
};

CCB_END()

#endif // !CCB_DATABASE_QUERY_HH
