/*
** Copyright 2014 Centreon
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

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/vars.hh"

using namespace com::centreon::broker;

/**
 *  This is a plugin similar to Centreon BAM official plugin. It reads
 *  from the /mod_bam/ table to provide a state, an output and some
 *  performance data.
 *
 *  @return 0 for OK, 1 for WARNING, 2 for CRITICAL and 3 for UNKNOWN.
 */
int main(int argc, char* argv[]) {
  int retval(3);
  try {
    // Extract command-line.
    if (argc < 3)
      throw(exceptions::msg() << "USAGE: " << argv[0] << " <dbname> <baid>");
    std::string db_name(argv[1]);
    unsigned int ba_id(strtoul(argv[2], NULL, 0));

    // Open database.
    QSqlDatabase db(QSqlDatabase::addDatabase("QMYSQL"));
    db.setHostName(DB_HOST);
    db.setPort(strtol(DB_PORT, NULL, 0));
    db.setUserName(DB_USER);
    db.setPassword(DB_PASSWORD);
    db.setDatabaseName(db_name.c_str());
    if (!db.open())
      throw(exceptions::msg()
            << "could not open DB: " << db.lastError().text());

    // Fetch result.
    QSqlQuery q(db);
    {
      std::ostringstream oss;
      oss << "SELECT current_status, current_level"
          << "  FROM mod_bam"
          << "  WHERE ba_id=" << ba_id;
      if (!q.exec(oss.str().c_str()) || !q.next())
        throw(exceptions::msg() << "could not get status of BA " << ba_id
                                << ": " << q.lastError().text());
    }

    // Plugin result.
    retval = q.value(0).toInt();
    std::cout << "BA " << ba_id << " has state " << retval << " and level "
              << q.value(1).toFloat()
              << "|BA_Level=" << static_cast<int>(q.value(1).toFloat())
              << "%\n";
  } catch (std::exception const& e) {
    std::cout << e.what() << "\n";
  } catch (...) {
    std::cout << "unknown exception\n";
  }
  QStringList l(QSqlDatabase::connectionNames());
  for (QStringList::const_iterator it(l.begin()), end(l.end()); it != end; ++it)
    QSqlDatabase::removeDatabase(*it);
  return (retval);
}
