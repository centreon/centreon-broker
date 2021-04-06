/*
** Copyright 2011-2021 Centreon
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

#include "com/centreon/broker/storage/stream.hh"

#include <cfloat>
#include <cmath>
#include <cstdlib>
#include <cstring>

#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/multiplexing/publisher.hh"
#include "com/centreon/broker/neb/host.hh"
#include "com/centreon/broker/neb/instance.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/service_status.hh"
#include "com/centreon/broker/storage/conflict_manager.hh"
#include "com/centreon/broker/storage/exceptions/perfdata.hh"
#include "com/centreon/broker/storage/index_mapping.hh"
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/parser.hh"
#include "com/centreon/broker/storage/perfdata.hh"
#include "com/centreon/broker/storage/remove_graph.hh"
#include "com/centreon/broker/storage/status.hh"
#include "com/centreon/exceptions/msg_fmt.hh"

using namespace com::centreon::exceptions;
using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::storage;

/**
 *  Constructor.
 *
 *  @param[in] db_cfg                  Database configuration.
 *  @param[in] rrd_len                 RRD length.
 *  @param[in] interval_length         Length in seconds of a time unit.
 *  @param[in] rebuild_check_interval  How often the stream must check
 *                                     for graph rebuild.
 *  @param[in] store_in_db             Should we insert data in
 *                                     data_bin ?
 *  @param[in] insert_in_index_data    Create entries in index_data or
 *                                     not.
 */
stream::stream(database_config const& dbcfg,
               uint32_t rrd_len,
               uint32_t interval_length,
               uint32_t rebuild_check_interval,
               bool store_in_db)
    : io::stream("storage"),
      _pending_events(0),
      _rebuilder(dbcfg,
                 rebuild_check_interval,
                 rrd_len ? rrd_len : 15552000,
                 interval_length),
      _stopped(false) {
  log_v2::sql()->debug("storage stream instanciation");
  if (!rrd_len)
    rrd_len = 15552000;

  if (!conflict_manager::init_storage(store_in_db, rrd_len, interval_length,
                                      dbcfg.get_queries_per_transaction()))
    throw msg_fmt(
        "storage: Unable to initialize the storage connection to the database");
}

int32_t stream::stop() {
  // Stop cleanup thread.
  int32_t retval =
      conflict_manager::instance().unload(conflict_manager::storage);
  log_v2::core()->info("storage stream stopped with {} acknowledged events",
                       retval);
  _stopped = true;
  return retval;
}

/**
 *  Destructor.
 */
stream::~stream() {
  assert(_stopped);
  // Stop cleanup thread.
  log_v2::sql()->trace("storage: stream destruction");
}

/**
 *  Flush the stream.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::flush() {
  int32_t retval =
      conflict_manager::instance().get_acks(conflict_manager::storage);
  _pending_events -= retval;

  // Event acknowledgement.
  log_v2::perfdata()->debug("storage: {} events have not yet been acknowledged",
                            _pending_events);
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
  throw broker::exceptions::shutdown("cannot read from a storage stream");
  return true;
}

/**
 *  Get endpoint statistics.
 *
 *  @param[out] tree Output tree.
 */
void stream::statistics(json11::Json::object& tree) const {
  std::lock_guard<std::mutex> lock(_statusm);
  if (!_status.empty())
    tree["status"] = _status;
  tree["pending events"] = _pending_events;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int32_t stream::write(std::shared_ptr<io::data> const& data) {
  ++_pending_events;

  assert(data);

  int32_t ack =
      conflict_manager::instance().send_event(conflict_manager::storage, data);
  _pending_events -= ack;
  return ack;
}

/**************************************
 *                                     *
 *           Private Methods           *
 *                                     *
 **************************************/

/**
 *  Update status of endpoint.
 *
 *  @param[in] status New status.
 */
void stream::_update_status(std::string const& status) {
  std::lock_guard<std::mutex> lock(_statusm);
  _status = status;
}
