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

#include "sql/connector.hh"
#include "sql/stream.hh"

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
connector::connector() {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c)
  : io::connector(c),
    _db(c._db),
    _host(c._host),
    _password(c._password),
    _type(c._type),
    _user(c._user) {}

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
  _db = c._db;
  _host = c._host;
  _password = c._password;
  _type = c._type;
  _user = c._user;
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
 *  @param[in] type     Database type.
 *  @param[in] host     Database host.
 *  @param[in] user     User.
 *  @param[in] password Password.
 *  @param[in] db       Database name.
 */
void connector::connect_to(QString const& type,
                           QString const& host,
                           QString const& user,
                           QString const& password,
                           QString const& db) {
  _db = db;
  _host = host;
  _password = password;
  _type = type;
  _user = user;
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @return SQL connection object.
 */
QSharedPointer<io::stream> connector::open() {
  return (QSharedPointer<io::stream>(new stream(_type,
            _host,
            _user,
            _password,
            _db)));
}
