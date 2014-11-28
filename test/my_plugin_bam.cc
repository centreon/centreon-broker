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

#include <cstdlib>
#include <iostream>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>
#include <sstream>
#include "com/centreon/broker/exceptions/msg.hh"
#include "test/config.hh"
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
      throw (exceptions::msg() << "USAGE: " << argv[0]
             << " <dbname> <baid>");
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
      throw (exceptions::msg() << "could not open DB: "
             << db.lastError().text());

    // Fetch result.
    QSqlQuery q(db);
    {
      std::ostringstream oss;
      oss << "SELECT current_status, current_level"
          << "  FROM mod_bam"
          << "  WHERE ba_id=" << ba_id;
      if (!q.exec(oss.str().c_str()) || !q.next())
        throw (exceptions::msg()
               << "could not get status of BA "
               << ba_id << ": " << q.lastError().text());
    }

    // Plugin result.
    retval = q.value(0).toInt();
    std::cout << "BA " << ba_id << " has state " << retval
              << " and level " << q.value(1).toFloat()
              << "|value=" << q.value(1).toFloat() << "\n";
  }
  catch (std::exception const& e) {
    std::cout << e.what() << "\n";
  }
  catch (...) {
    std::cout << "unknown exception\n";
  }
  QStringList l(QSqlDatabase::connectionNames());
  for (QStringList::const_iterator it(l.begin()), end(l.end());
       it != end;
       ++it)
    QSqlDatabase::removeDatabase(*it);
  return (retval);
}
