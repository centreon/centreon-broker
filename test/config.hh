/*
** Copyright 2012-2014 Merethis
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

#ifndef TEST_CONFIG_HH
#  define TEST_CONFIG_HH

#  include <list>
#  include <memory>
#  include <QSqlDatabase>
#  include "com/centreon/engine/objects.hh"

/**
 *  Test database.
 */
class           test_db {
public:
                test_db();
                ~test_db();
  QSqlDatabase* bi_db();
  QSqlDatabase* centreon_db();
  QSqlDatabase* storage_db();
  void          close();
  void          open(
                  char const* storage_db_name,
                  char const* bi_db_name = NULL,
                  char const* centreon_db_name = NULL);

private:
                test_db(test_db const& other);
  test_db&      operator=(test_db const& other);
  void          _close(std::auto_ptr<QSqlDatabase>& db);
  void          _open(QSqlDatabase& db, char const* db_name);
  void          _run_script(QSqlDatabase& db, char const* script_name);

  std::auto_ptr<QSqlDatabase>
                _bi;
  std::auto_ptr<QSqlDatabase>
                _centreon;
  std::auto_ptr<QSqlDatabase>
                _storage;
};

void            config_remove(char const* path);
void            config_write(
                  char const* path,
                  char const* more_directives = NULL,
                  std::list<host>* hosts = NULL,
                  std::list<service>* services = NULL,
                  std::list<command>* commands = NULL,
                  std::list<hostgroup>* host_groups = NULL,
                  std::list<servicegroup>* service_groups = NULL,
                  std::list<hostdependency>* host_deps = NULL,
                  std::list<servicedependency>* service_deps = NULL);

#endif // !TEST_CONFIG_HH
