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

#include "com/centreon/broker/bam/connector.hh"
#include "com/centreon/broker/bam/monitoring_stream.hh"
#include "com/centreon/broker/bam/reporting_stream.hh"
#include "com/centreon/broker/bam/sql_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() : io::endpoint(false), _type(bam_type) {}

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
 *  @param[in] type                    BAM stream type.
 *  @param[in] db_type                 BAM DB type.
 *  @param[in] db_host                 BAM DB host.
 *  @param[in] db_port                 BAM DB port.
 *  @param[in] db_user                 BAM DB user.
 *  @param[in] db_password             BAM DB password.
 *  @param[in] db_name                 BAM DB name.
 *  @param[in] queries_per_transaction Queries per transaction.
 *  @param[in] check_replication       Check for replication status or
 *                                     not.
 */
void connector::connect_to(
                  stream_type type,
                  QString const& db_type,
                  QString const& db_host,
                  unsigned short db_port,
                  QString const& db_user,
                  QString const& db_password,
                  QString const& db_name,
                  unsigned int queries_per_transaction,
                  bool check_replication) {
  _type = type;
  _db_type = plain_db_to_qt(db_type);
  _db_host = db_host;
  _db_port = db_port;
  _db_user = db_user;
  _db_password = db_password;
  _db_name = db_name;
  _queries_per_transaction = queries_per_transaction;
  _check_replication = check_replication;
  return ;
}

/**
 *  Connect to a DB.
 *
 *  @return BAM connection object.
 */
misc::shared_ptr<io::stream> connector::open() {
  if (_type == bam_bi_type) {
    misc::shared_ptr<reporting_stream>
      s(new reporting_stream(
              _db_type,
              _db_host,
              _db_port,
              _db_user,
              _db_password,
              _db_name,
              _queries_per_transaction,
              _check_replication));
    return (s.staticCast<io::stream>());
  }
  else {
    misc::shared_ptr<monitoring_stream>
      s(new monitoring_stream(
              _db_type,
              _db_host,
              _db_port,
              _db_user,
              _db_password,
              _db_name,
              _queries_per_transaction,
              _check_replication));
    s->initialize();
    return (s.staticCast<io::stream>());
  }
}

/**
 *  Connect to a DB.
 *
 *  @param[in] id Unused.
 *
 *  @return BAM connection object.
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
  _db_name = c._db_name;
  _db_host = c._db_host;
  _db_password = c._db_password;
  _db_port = c._db_port;
  _db_user = c._db_user;
  _db_type = c._db_type;
  _queries_per_transaction = c._queries_per_transaction;
  _type = c._type;
  return ;
}
