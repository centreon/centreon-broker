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

#include <map>
#include "com/centreon/broker/bam/sql_mapping.hh"

using namespace com::centreon::broker;

 /**
 *  This function maps the logical name for a RDBMS to the QT lib name.
 *
 *  @param[in]  The logical name for the database.
 *
 *  @return     The QT lib name for the database system.
 */
QString bam::plain_db_to_qt(QString const& dbtype) {
  // Mapping.
  static struct {
    QString plain;
    QString qt;
  } const type_mapping[] = {
    { "db2",        "QDB2" },
    { "ibase",      "QIBASE" },
    { "interbase",  "QIBASE" },
    { "mysql",      "QMYSQL" },
    { "oci",        "QOCI" },
    { "oracle",     "QOCI" },
    { "odbc",       "QODBC" },
    { "psql",       "QPSQL" },
    { "postgres",   "QPSQL" },
    { "postgresql", "QPSQL" },
    { "sqlite",     "QSQLITE" },
    { "tds",        "QTDS" },
    { "sybase",     "QTDS" }
  };

  // Lower case the string.
  QString plain_db_type(dbtype.toLower());

  // Browse type mapping.
  for (int i(0); i < sizeof(type_mapping) / sizeof(*type_mapping); ++i)
    if (type_mapping[i].plain == plain_db_type)
      return (type_mapping[i].qt);
  return (dbtype);
}
