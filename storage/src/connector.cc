/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/storage/connector.hh"
#include "com/centreon/broker/storage/stream.hh"

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
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() : io::endpoint(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
  : io::endpoint(c) {
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
  if (this != &c) {
    io::endpoint::operator=(c);
    _internal_copy(c);
  }
  return (*this);
}

/**
 *  Close the connector.
 */
void connector::close() {
  return ;
}

/**
 *  Set connection parameters.
 *
 *  @param[in] storage_type            Storage DB type.
 *  @param[in] storage_host            Storage DB host.
 *  @param[in] storage_port            Storage DB port.
 *  @param[in] storage_user            Storage DB user.
 *  @param[in] storage_password        Storage DB password.
 *  @param[in] storage_db              Storage DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] rrd_len                 RRD storage length.
 *  @param[in] interval_length         Interval length.
 */
void connector::connect_to(
                  QString const& storage_type,
                  QString const& storage_host,
                  unsigned short storage_port,
                  QString const& storage_user,
                  QString const& storage_password,
                  QString const& storage_db,
                  unsigned int queries_per_transaction,
                  unsigned int rrd_len,
                  time_t interval_length) {
  _queries_per_transaction = queries_per_transaction;
  _storage_db = storage_db;
  _storage_host = storage_host;
  _storage_password = storage_password;
  _storage_port = storage_port;
  _storage_user = storage_user;
  to_qt_sql_type(storage_type, _storage_type);
  _rrd_len = rrd_len;
  _interval_length = interval_length;
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @return Storage connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(
            new stream(
                  _storage_type,
                  _storage_host,
                  _storage_port,
                  _storage_user,
                  _storage_password,
                  _storage_db,
                  _queries_per_transaction,
                  _rrd_len,
                  _interval_length)));
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
  _interval_length = c._interval_length;
  _queries_per_transaction = c._queries_per_transaction;
  _rrd_len = c._rrd_len;
  _storage_db = c._storage_db;
  _storage_host = c._storage_host;
  _storage_password = c._storage_password;
  _storage_port = c._storage_port;
  _storage_user = c._storage_user;
  _storage_type = c._storage_type;
  return ;
}
