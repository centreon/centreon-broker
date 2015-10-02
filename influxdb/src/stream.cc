/*
** Copyright 2011-2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
          std::string const& version,
          std::string const& status_ts,
          std::vector<column> const& status_cols,
          std::string const& metric_ts,
          std::vector<column> const& metric_cols,
          misc::shared_ptr<persistent_cache> const& cache)
  : _user(user),
    _password(passwd),
    _address(addr),
    _port(port),
    _db(db),
    _queries_per_transaction(queries_per_transaction == 0 ?
                               1 : queries_per_transaction),
    _pending_queries(0),
    _actual_query(0),
    _commit(false),
    _cache(cache) {
  if (version == "0.9")
    _influx_db.reset(new influxdb9(
                           user,
                           passwd,
                           addr,
                           port,
                           db,
                           status_ts,
                           status_cols,
                           metric_ts,
                           metric_cols,
                           _cache));
  else
    throw (exceptions::msg()
           << "influxdb: unrecognized influxdb version '" << version << "'");
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Read from the datbase.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.clear();
  throw (com::centreon::broker::io::exceptions::shutdown(true, false)
         << "cannot read from InfluxDB database");
  return (true);
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(io::properties& tree) const {
  QMutexLocker lock(&_statusm);
  if (!_status.empty())
    tree.add_property("status", io::property("status", _status));
  return ;
}

/**
 *  Do nothing.
 */
void stream::update() {
  return ;
}

/**
 *  Flush the stream.
 */
void stream::flush() {
  _commit = true;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(misc::shared_ptr<io::data> const& data) {
  if (!validate(data, "influxdb"))
    return (1);

  ++_pending_queries;

  // Give data to cache.
  _cache.write(data);

  // Process metric events.
  if (data->type()
        == io::events::data_type<io::events::storage,
                                 storage::de_metric>::value) {
    _influx_db->write(data.ref_as<storage::metric const>());
    ++_actual_query;
  }
  else if (data->type()
           == io::events::data_type<io::events::storage,
                                    storage::de_status>::value) {
    _influx_db->write(data.ref_as<storage::status const>());
    ++_actual_query;
  }
  if (_actual_query >= _queries_per_transaction)
    _commit = true;

  if (_commit) {
    logging::debug(logging::medium)
      << "influxdb: commiting " << _actual_query << " queries";
    unsigned int ret = _pending_queries;
    _actual_query = 0;
    _pending_queries = 0;
    _influx_db->commit();
    _commit = false;
    return (ret);
  }
  else
    return (0);
}
