/*
** Copyright 2011-2015 Merethis
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

#include "com/centreon/broker/graphite/connector.hh"
#include "com/centreon/broker/graphite/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::graphite;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector()
  : io::endpoint(false) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
connector::connector(connector const& other)
  : io::endpoint(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& other) {
  if (this != &other) {
    io::endpoint::operator=(other);
    _internal_copy(other);
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
 */
void connector::connect_to(
                  std::string const& metric_naming,
                  std::string const& status_naming,
                  std::string const& db_user,
                  std::string const& db_passwd,
                  std::string const& db_addr,
                  unsigned short db_port,
                  unsigned int queries_per_transaction,
                  misc::shared_ptr<persistent_cache> const& cache) {
  _metric_naming = metric_naming;
  _status_naming = status_naming;
  _user = db_user;
  _password = db_passwd;
  _addr = db_addr;
  _port = db_port,
  _queries_per_transaction = queries_per_transaction;
  _persistent_cache = cache;
  return ;
}

/**
 *  Connect to a graphite DB.
 *
 *  @return Graphite connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(
            new stream(
                  _metric_naming,
                  _status_naming,
                  _user,
                  _password,
                  _addr,
                  _port,
                  _queries_per_transaction,
                  _persistent_cache)));
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void connector::_internal_copy(connector const& other) {
  _metric_naming = other._metric_naming;
  _status_naming = other._status_naming;
  _user = other._user;
  _password = other._password;
  _addr = other._addr;
  _port = other._port;
  _queries_per_transaction = other._queries_per_transaction;
  _persistent_cache = other._persistent_cache;
  return ;
}
