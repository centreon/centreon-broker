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

#include "com/centreon/broker/storage/connector.hh"
#include "com/centreon/broker/storage/stream.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

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
 *  Set connection parameters.
 *
 *  @param[in] db_cfg                  Database configuration.
 *  @param[in] rrd_len                 RRD storage length.
 *  @param[in] interval_length         Interval length.
 *  @param[in] rebuild_check_interval  How often the storage endpoint
 *                                     must check for graph rebuild.
 *  @param[in] store_in_data_bin       True to store performance data in
 *                                     the data_bin table.
 *  @param[in] insert_in_index_data    Create entries in index_data.
 */
void connector::connect_to(
                  database_config const& db_cfg,
                  unsigned int rrd_len,
                  time_t interval_length,
                  unsigned int rebuild_check_interval,
                  bool store_in_data_bin,
                  bool insert_in_index_data) {
  _db_cfg = db_cfg;
  _rrd_len = rrd_len;
  _interval_length = interval_length;
  _rebuild_check_interval = rebuild_check_interval;
  _store_in_data_bin = store_in_data_bin;
  _insert_in_index_data = insert_in_index_data;
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @param[in] cache  Persistent cache is not used by the storage
 *                    module.
 *
 *  @return Storage connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(
            new stream(
                  _db_cfg,
                  _rrd_len,
                  _interval_length,
                  _rebuild_check_interval,
                  _store_in_data_bin,
                  _insert_in_index_data)));
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
  _db_cfg = other._db_cfg;
  _insert_in_index_data = other._insert_in_index_data;
  _interval_length = other._interval_length;
  _rebuild_check_interval = other._rebuild_check_interval;
  _rrd_len = other._rrd_len;
  _store_in_data_bin = other._store_in_data_bin;
  return ;
}
