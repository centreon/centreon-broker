/*
** Copyright 2011 Merethis
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

#include "exceptions/basic.hh"
#include "storage/connector.hh"
#include "storage/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

/**
 *  Extract the DB type and set it to be Qt-compatible.
 *
 *  @param[in]  type Original type string.
 *  @param[out] out  Output.
 */
static void to_qt_sql_type(QString const& type, QString& out) {
  if (!type.compare("db2", Qt::CaseInsensitive))
    out = "QDB2";
  else if (!type.compare("ibase", Qt::CaseInsensitive)
           || !type.compare("interbase", Qt::CaseInsensitive))
    out = "QIBASE";
  else if (!type.compare("mysql", Qt::CaseInsensitive))
    out = "QMYSQL";
  else if (!type.compare("oci", Qt::CaseInsensitive)
           || !type.compare("oracle", Qt::CaseInsensitive))
    out = "QOCI";
  else if (!type.compare("odbc", Qt::CaseInsensitive))
    out = "QODBC";
  else if (!type.compare("psql", Qt::CaseInsensitive)
           || !type.compare("postgres", Qt::CaseInsensitive)
           || !type.compare("postgresql", Qt::CaseInsensitive))
    out = "QPSQL";
  else if (!type.compare("sqlite", Qt::CaseInsensitive))
    out = "QSQLITE";
  else if (!type.compare("tds", Qt::CaseInsensitive)
           || !type.compare("sybase", Qt::CaseInsensitive))
    out = "QTDS";
  else
    out = type;
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] c Object to copy.
 */
void connector::_internal_copy(connector const& c) {
  _centreon_db = c._centreon_db;
  _centreon_host = c._centreon_host;
  _centreon_password = c._centreon_password;
  _centreon_user = c._centreon_user;
  _centreon_type = c._centreon_type;
  _storage_db = c._storage_db;
  _storage_host = c._storage_host;
  _storage_password = c._storage_password;
  _storage_user = c._storage_user;
  _storage_type = c._storage_type;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
  : io::connector(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  io::connector::operator=(c);
  _internal_copy(c);
  return (*this);
}

/**
 *  Close the connector.
 */
void connector::close() {
  return ;
}

/**
 *  Connect a lower layer stream.
 *
 *  @param[in] ptr Unused.
 */
void connector::connect(QSharedPointer<io::stream> ptr) {
  (void)ptr;
  throw (exceptions::basic() << "cannot connect through a storage connect (software bug)");
  return ;
}

/**
 *  Set connection parameters.
 *
 *  @param[in] centreon_type     Centreon DB type.
 *  @param[in] centreon_host     Centreon DB host.
 *  @param[in] centreon_user     Centreon DB user.
 *  @param[in] centreon_password Centreon DB password.
 *  @param[in] centreon_db       Centreon DB name.
 *  @param[in] storage_type      Storage DB type.
 *  @param[in] storage_host      Storage DB host.
 *  @param[in] storage_user      Storage DB user.
 *  @param[in] storage_password  Storage DB password.
 *  @param[in] storage_db        Storage DB name.
 */
void connector::connect_to(QString const& centreon_type,
                           QString const& centreon_host,
                           QString const& centreon_user,
                           QString const& centreon_password,
                           QString const& centreon_db,
                           QString const& storage_type,
                           QString const& storage_host,
                           QString const& storage_user,
                           QString const& storage_password,
                           QString const& storage_db) {
  _centreon_db = centreon_db;
  _centreon_host = centreon_host;
  _centreon_password = centreon_password;
  _centreon_user = centreon_user;
  to_qt_sql_type(centreon_type, _centreon_type);
  _storage_db = storage_db;
  _storage_host = storage_host;
  _storage_password = storage_password;
  _storage_user = storage_user;
  to_qt_sql_type(storage_type, _storage_type);
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @return Storage connection object.
 */
QSharedPointer<io::stream> connector::open() {
  return (QSharedPointer<io::stream>(new stream(_centreon_type,
            _centreon_host,
            _centreon_user,
            _centreon_password,
            _centreon_db,
            _storage_type,
            _storage_host,
            _storage_user,
            _storage_password,
            _storage_db)));
}
