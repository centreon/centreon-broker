/*
** Copyright 2011-2014 Merethis
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

#include <QMutexLocker>
#include <sstream>
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/io/exceptions/shutdown.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/influxdb/stream.hh"
#include "com/centreon/broker/influxdb/influxdb9.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 */
stream::stream(
          std::string const& user,
          std::string const& passwd,
          std::string const& addr,
          unsigned short port,
          std::string const& db,
          unsigned int queries_per_transaction,
          std::string const& version)
  : _process_out(true),
    _user(user),
    _password(passwd),
    _address(addr),
    _port(port),
    _db(db),
    _queries_per_transaction(queries_per_transaction),
    _actual_query(0) {
  if (version == "0.9")
    _influx_db.reset(new influxdb9(user, passwd, addr, port, db));
  else
    throw (exceptions::msg()
           << "influxdb: unrecognized influxdb version '" << version << "'");
}

/**
 *  Destructor.
 */
stream::~stream() {
}

/**
 *  Enable or disable output event processing.
 *
 *  @param[in] in  Unused.
 *  @param[in] out Set to true to enable output event processing.
 */
void stream::process(bool in, bool out) {
  _process_out = in || !out; // Only for immediate shutdown.
  return ;
}

/**
 *  Read from the datbase.
 *
 *  @param[out] d Cleared.
 */
void stream::read(misc::shared_ptr<io::data>& d) {
  d.clear();
  throw (com::centreon::broker::exceptions::msg()
         << "influxdb: attempt to read from an influxdb stream (not supported yet)");
  return ;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty()) {
    io::property& p(tree["status"]);
    p.set_perfdata(_status);
    p.set_graphable(false);
  }
  return ;
}

/**
 *  Do nothing.
 */
void stream::update() {
  return ;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
unsigned int stream::write(misc::shared_ptr<io::data> const& data) {
  // Check that processing is enabled.
  if (!_process_out)
    throw (io::exceptions::shutdown(true, true)
             << "influxdb stream is shutdown");

  bool commit = false;

  // Process metric events.
  if (!data.isNull()) {
    if (data->type()
          == io::events::data_type<io::events::storage,
                                   storage::de_metric>::value) {
      _influx_db->write(data.ref_as<storage::metric const>());
      ++_actual_query;
      if (_actual_query >= _queries_per_transaction)
        commit = true;
    }
  }
  else
    commit = true;

  if (commit) {
    logging::debug(logging::medium)
      << "influxdb: commiting " << _actual_query << " queries";
    unsigned int ret = _actual_query;
    _actual_query = 0;
    _influx_db->commit();
    return (ret);
  }
  else
    return (0);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/
