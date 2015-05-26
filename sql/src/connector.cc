/*
** Copyright 2011-2012,2015 Merethis
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

#include "com/centreon/broker/sql/connector.hh"
#include "com/centreon/broker/sql/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::sql;

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
  : io::endpoint(c),
    _check_replication(c._check_replication),
    _cleanup_check_interval(c._cleanup_check_interval),
    _db(c._db),
    _host(c._host),
    _instance_timeout(c._instance_timeout),
    _password(c._password),
    _port(c._port),
    _queries_per_transaction(0),
    _type(c._type),
    _user(c._user),
    _with_state_events(c._with_state_events) {}

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
    _check_replication = c._check_replication;
    _cleanup_check_interval = c._cleanup_check_interval;
    _db = c._db;
    _host = c._host;
    _instance_timeout = c._instance_timeout;
    _password = c._password;
    _port = c._port;
    _queries_per_transaction = c._queries_per_transaction;
    _type = c._type;
    _user = c._user;
    _with_state_events = c._with_state_events;
  }
  return (*this);
}

/**
 *  Clone the connector.
 *
 *  @return This object.
 */
io::endpoint* connector::clone() const {
  return (new connector(*this));
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
 *  @param[in] type                    Database type.
 *  @param[in] host                    Database host.
 *  @param[in] port                    Database port.
 *  @param[in] user                    User.
 *  @param[in] password                Password.
 *  @param[in] db                      Database name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] cleanup_check_interval  How often the stream must
 *                                     check for cleanup database.
 *  @param[in] check_replication       true to check replication status.
 *  @param[in] with_state_events       Enable state events ?
 */
void connector::connect_to(
                  QString const& type,
                  QString const& host,
                  unsigned short port,
                  QString const& user,
                  QString const& password,
                  QString const& db,
                  unsigned int queries_per_transaction,
                  unsigned int cleanup_check_interval,
                  unsigned int instance_timeout,
                  bool check_replication,
                  bool with_state_events) {
  _cleanup_check_interval = cleanup_check_interval;
  _instance_timeout = instance_timeout;
  _check_replication = check_replication;
  _db = db;
  _host = host;
  _password = password;
  _port = port;
  _queries_per_transaction = queries_per_transaction;
  _type = type;
  _user = user;
  _with_state_events = with_state_events;
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @return SQL connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(new stream(
                                             _type.toStdString(),
                                             _host.toStdString(),
                                             _port,
                                             _user.toStdString(),
                                             _password.toStdString(),
                                             _db.toStdString(),
                                             _queries_per_transaction,
                                             _cleanup_check_interval,
                                             _instance_timeout,
                                             _check_replication,
                                             _with_state_events)));
}
