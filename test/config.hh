/*
** Copyright 2012-2014 Centreon
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
  QSqlDatabase* centreon_db();
  QSqlDatabase* storage_db();
  void          close();
  void          open(
                  char const* storage_db_name,
                  char const* bi_db_name = NULL,
                  char const* centreon_db_name = NULL);

  void          set_remove_db_on_close(bool val);

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
                  std::list<hostgroup>* host_groups = NULL,
                  std::list<servicegroup>* service_groups = NULL,
                  std::list<hostdependency>* host_deps = NULL,
                  std::list<servicedependency>* service_deps = NULL);

#endif // !TEST_CONFIG_HH
