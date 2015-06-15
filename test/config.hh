/*
** Copyright 2012-2015 Merethis
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
#  include <map>
#  include <memory>
#  include <QSqlDatabase>
#  include <string>
#  include "com/centreon/engine/objects.hh"

/**
 *  Test database.
 */
class           test_db {
public:
                test_db();
                ~test_db();
  QSqlDatabase* bi_db();
  void          bi_run(
                  QString const& query,
                  QString const& error_msg);
  QSqlDatabase* centreon_db();
  void          centreon_run(
                  QString const& query,
                  QString const& error_msg);
  void          close();
  void          open(
                  char const* centreon_db_name,
                  char const* bi_db_name = NULL,
                  bool with_cfg_tables = false);

  void          set_remove_db_on_close(bool val);

private:
                test_db(test_db const& other);
  test_db&      operator=(test_db const& other);
  void          _close(std::auto_ptr<QSqlDatabase>& db);
  void          _open(QSqlDatabase& db, char const* db_name);
  void          _run_query(
                   QSqlDatabase* db,
                   QString const& query,
                   QString const& error_msg);
  void          _run_script(QSqlDatabase& db, char const* script_name);

  std::auto_ptr<QSqlDatabase>
                _bi;
  std::auto_ptr<QSqlDatabase>
                _centreon;

  bool          _remove_db_on_close;
};

/**
 *  Test file.
 */
class                test_file {
public:
                     test_file();
                     test_file(test_file const& other);
                     ~test_file();
  test_file&         operator=(test_file const& other);
  void               close();
  std::string const& generate();
  void               set(
                       std::string const& variable,
                       std::string const& value);
  void               set_template(std::string const& base_file);

private:
  void               _internal_copy(test_file const& other);

  std::string        _base_file;
  std::string        _target_file;
  std::map<std::string, std::string>
                     _variables;
};

void            config_remove(char const* path);
void            config_write(
                  char const* path,
                  char const* more_directives = NULL,
                  std::list<host>* hosts = NULL,
                  std::list<service>* services = NULL,
                  std::list<command>* commands = NULL,
                  std::list<hostdependency>* host_deps = NULL,
                  std::list<servicedependency>* service_deps = NULL);

#endif // !TEST_CONFIG_HH
