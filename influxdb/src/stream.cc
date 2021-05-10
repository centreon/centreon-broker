/*
** Copyright 2011-2017 Centreon
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

#include "com/centreon/broker/influxdb/stream.hh"
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/influxdb/influxdb12.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/global_lock.hh"
#include "com/centreon/broker/multiplexing/engine.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::influxdb;

/**
 *  Constructor.
 *
 */
stream::stream(std::string const& user,
               std::string const& passwd,
               std::string const& addr,
               unsigned short port,
               std::string const& db,
               uint32_t queries_per_transaction,
               std::string const& status_ts,
               std::vector<column> const& status_cols,
               std::string const& metric_ts,
               std::vector<column> const& metric_cols,
               std::shared_ptr<persistent_cache> const& cache)
    : io::stream("influxdb"),
      _user(user),
      _password(passwd),
      _address(addr),
      _db(db),
      _queries_per_transaction(
          queries_per_transaction == 0 ? 1 : queries_per_transaction),
      _pending_queries(0),
      _actual_query(0),
      _commit(false),
      _cache(cache) {
  _influx_db.reset(new influxdb12(user, passwd, addr, port, db, status_ts,
                                  status_cols, metric_ts, metric_cols, _cache));
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::flush() {
  logging::debug(logging::medium)
      << "influxdb: commiting " << _actual_query << " queries";
  int ret(_pending_queries);
  _actual_query = 0;
  _pending_queries = 0;
  _influx_db->commit();
  _commit = false;
  return ret;
}

/**
 * @brief Flush the stream and stop it.
 *
 * @return Number of acknowledged events.
 */
int32_t stream::stop() {
  int32_t retval = flush();
  log_v2::core()->info("influxdb stream stopped with {} acknowledged events",
                       retval);
  return retval;
}

/**
 *  Read from the datbase.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  throw exceptions::shutdown("cannot read from InfluxDB database");
  return true;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(nlohmann::json& tree) const {
  std::lock_guard<std::mutex> lock(_statusm);
  if (!_status.empty())
    tree["status"] = _status;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(std::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_queries;
  if (!validate(data, get_name()))
    return 0;

  // Give data to cache.
  _cache.write(data);

  // Process metric events.
  if (data->type() ==
      io::events::data_type<io::events::storage, storage::de_metric>::value) {
    _influx_db->write(*std::static_pointer_cast<storage::metric const>(data));
    ++_actual_query;
  } else if (data->type() == io::events::data_type<io::events::storage,
                                                   storage::de_status>::value) {
    _influx_db->write(*std::static_pointer_cast<storage::status const>(data));
    ++_actual_query;
  }
  if (_actual_query >= _queries_per_transaction)
    _commit = true;

  if (_commit)
    return flush();
  else
    return 0;
}
