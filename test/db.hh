/*
** Copyright 2012-2015 Centreon
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

#ifndef CCB_TEST_DB_HH
#  define CCB_TEST_DB_HH

#  include <memory>
#  include <QSqlDatabase>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "test/predicate.hh"

#  define RTMONITORING_TABLES_V2 "instances", \
                                 "hosts", \
                                 "hostgroups", \
                                 "hosts_hostgroups", \
                                 "hosts_hosts_dependencies", \
                                 "hosts_hosts_parents", \
                                 "services", \
                                 "servicegroups", \
                                 "services_servicegroups", \
                                 "services_services_dependencies", \
                                 "acknowledgements", \
                                 "comments", \
                                 "customvariables", \
                                 "downtimes", \
                                 "eventhandlers", \
                                 "flappingstatuses", \
                                 "issues", \
                                 "issues_issues_parents", \
                                 "logs", \
                                 "modules", \
                                 "hoststateevents", \
                                 "servicestateevents"

CCB_BEGIN()

namespace         test {
  /**
   *  Test database.
   */
  class           db {
  public:
                  db(
                    std::string const& name,
                    char const* const* include = NULL,
                    char const* const* exclude = NULL);
                  ~db();
    template      <int columns>
    void          check_content(
                    std::string const& query,
                    predicate expected[][columns]);
    void          close();
    QSqlDatabase* get_db();
    void          run(
                    QString const& query,
                    QString const& error_msg);
    void          set_remove_db_on_close(bool val = true);

  private:
                  db(db const& other);
    db&           operator=(db const& other);
    void          _close(std::auto_ptr<QSqlDatabase>& db);
    void          _open(QSqlDatabase& db, char const* db_name);
    void          _run_script(char const* script_name);

    std::auto_ptr<QSqlDatabase>
                  _db;
    bool          _remove_db_on_close;
  };
}

CCB_END()

#endif // !CCB_TEST_DB_HH
