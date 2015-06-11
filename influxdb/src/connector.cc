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

#include "com/centreon/broker/influxdb/connector.hh"
#include "com/centreon/broker/influxdb/stream.hh"
#include "com/centreon/broker/persistent_cache.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

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
                  std::string const& user,
                  std::string const& passwd,
                  std::string const& addr,
                  unsigned short port,
                  std::string const& db,
                  unsigned int queries_per_transaction,
                  std::string const& version,
                  std::string const& status_ts,
                  std::vector<column> const& status_cols,
                  std::string const& metric_ts,
                  std::vector<column> const& metric_cols,
                  misc::shared_ptr<persistent_cache> const& cache) {
  _user = user;
  _password = passwd;
  _addr = addr;
  _port = port,
  _db = db;
  _queries_per_transaction = queries_per_transaction;
  _version = version;
  _status_ts = status_ts;
  _status_cols = status_cols;
  _metric_ts = metric_ts;
  _metric_cols = metric_cols;
  _cache = cache;
  return ;
}

/**
 *  Connect to an influxdb DB.
 *
 *  @return Influxdb connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(
            new stream(
                  _user,
                  _password,
                  _addr,
                  _port,
                  _db,
                  _queries_per_transaction,
                  _version,
                  _status_ts,
                  _status_cols,
                  _metric_ts,
                  _metric_cols,
                  _cache)));
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
  _user = other._user;
  _password = other._password;
  _addr = other._addr;
  _port = other._port;
  _db = other._db;
  _queries_per_transaction = other._queries_per_transaction;
  _version = other._version;
  _status_ts = other._status_ts;
  _status_cols = other._status_cols;
  _metric_ts = other._metric_ts;
  _metric_cols = other._metric_cols;
  _cache = other._cache;
  return ;
}
