/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_DATABASE_QUERY_HH
#  define CCB_DATABASE_QUERY_HH

#  include <QSqlQuery>
#  include <QString>
#  include <QVariant>
#  include <set>
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
  typedef std::set<std::string> excluded_fields;

                    database_query(database& db);
                    ~database_query();
  database_query&   operator<<(io::data const& e);
  void              bind_value(
                      QString const& placeholder,
                      QVariant const& value);
  database const&   db_object() const;
  void              finish();
  QVariant          last_insert_id();
  int               num_rows_affected();
  void              set_excluded(excluded_fields const& excluded);
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
  excluded_fields   _excluded;
};

CCB_END()

#endif // !CCB_DATABASE_QUERY_HH
