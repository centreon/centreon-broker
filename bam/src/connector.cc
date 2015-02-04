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
 *  @param[in] type             BAM stream type.
 *  @param[in] db_cfg           Database configuration.
 *  @param[in] ext_cmd_file     External command file.
 *  @param[in] storage_db_name  Storage DB name.
 */
void connector::connect_to(
                  stream_type type,
                  database_config const& db_cfg,
                  std::string const& ext_cmd_file,
                  std::string const& storage_db_name) {
  _type = type;
  _db_cfg = db_cfg;
  _ext_cmd_file = ext_cmd_file;
  _storage_db_name = storage_db_name;
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
      s(new reporting_stream(_db_cfg));
    return (s.staticCast<io::stream>());
  }
  else {
    misc::shared_ptr<monitoring_stream>
      s(new monitoring_stream(
               _db_cfg,
               _ext_cmd_file,
               _storage_db_name));
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
 *  @param[in] other  Object to copy.
 */
void connector::_internal_copy(connector const& other) {
  _db_cfg = other._db_cfg;
  _ext_cmd_file = other._ext_cmd_file;
  _storage_db_name = other._storage_db_name;
  _type = other._type;
  return ;
}
