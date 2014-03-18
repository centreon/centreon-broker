/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/bam/connector.hh"
#include "com/centreon/broker/bam/stream.hh"
#include "com/centreon/broker/bam/sql_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/



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
 *  @param[in] bam_type            Bam DB type.
 *  @param[in] bam_host            Bam DB host.
 *  @param[in] bam_port            Bam DB port.
 *  @param[in] bam_user            Bam DB user.
 *  @param[in] bam_password        Bam DB password.
 *  @param[in] bam_db              Bam DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] rrd_len                 RRD bam length.
 *  @param[in] interval_length         Interval length.
 *  @param[in] rebuild_check_interval  How often the bam endpoint
 *                                     must check for graph rebuild.
 *  @param[in] check_replication       Check for replication status or
 *                                     not.
 *  @param[in] store_in_data_bin       True to store performance data in
 *                                     the data_bin table.
 *  @param[in] insert_in_index_data    Create entries in index_data.
 */
void connector::connect_to(
                  QString const& bam_type,
                  QString const& bam_host,
                  unsigned short bam_port,
                  QString const& bam_user,
                  QString const& bam_password,
                  QString const& bam_db,
                  unsigned int queries_per_transaction,
                  unsigned int rrd_len,
                  time_t interval_length,
                  unsigned int rebuild_check_interval,
                  bool check_replication,
                  bool store_in_data_bin,
                  bool insert_in_index_data) {
  _queries_per_transaction = queries_per_transaction;
  _bam_db = bam_db;
  _bam_host = bam_host;
  _bam_password = bam_password;
  _bam_port = bam_port;
  _bam_user = bam_user;
  _bam_type = map_2_qt(bam_type);
  _rrd_len = rrd_len;
  _interval_length = interval_length;
  _rebuild_check_interval = rebuild_check_interval;
  _check_replication = check_replication;
  _store_in_data_bin = store_in_data_bin;
  _insert_in_index_data = insert_in_index_data;
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @return Bam connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  return (misc::shared_ptr<io::stream>(
            new stream(
                  _bam_type,
                  _bam_host,
                  _bam_port,
                  _bam_user,
                  _bam_password,
                  _bam_db,
                  _queries_per_transaction,
                  _rrd_len,
                  _interval_length,
                  _rebuild_check_interval,
                  _store_in_data_bin,
                  _check_replication,
                  _insert_in_index_data)));
}

/**
 *  Connect to a DB.
 *
 *  @param[in] id Unused.
 *
 *  @return Bam connection object.
 */
misc::shared_ptr<io::stream> connector::open(QString const& id) {
  (void)id;
  return (open());
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
  _check_replication = c._check_replication;
  _insert_in_index_data = c._insert_in_index_data;
  _interval_length = c._interval_length;
  _queries_per_transaction = c._queries_per_transaction;
  _rebuild_check_interval = c._rebuild_check_interval;
  _rrd_len = c._rrd_len;
  _bam_db = c._bam_db;
  _bam_host = c._bam_host;
  _bam_password = c._bam_password;
  _bam_port = c._bam_port;
  _bam_user = c._bam_user;
  _bam_type = c._bam_type;
  _store_in_data_bin = c._store_in_data_bin;
  return ;
}
