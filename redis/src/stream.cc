/*
** Copyright 2018 Centreon
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
#include <sstream>
#include "com/centreon/broker/exceptions/shutdown.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/redis/redisdb.hh"
#include "com/centreon/broker/redis/stream.hh"
#include "com/centreon/broker/neb/host_status.hh"
#include "com/centreon/broker/neb/service_status.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::misc;
using namespace com::centreon::broker::redis;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] addr                    Address to connect to
 *  @param[in] port                    port
 */
stream::stream(
          std::string const& address,
          unsigned short port,
          std::string const& password)
  : _redisdb(new redisdb(address, port, password)),
    _pending_queries(0),
    _actual_query(0),
    _queries_per_transaction(1) {}

/**
 *  Read from the connector.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(misc::shared_ptr<io::data> const& data) {
  // Take this event into account.
  ++_pending_queries;
  if (!validate(data, "redis"))
    return 0;

  unsigned int type(data->type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));
  // Process event.
  if (cat == io::events::neb) {
    switch (elem) {
      case 12:
        // Host
        _redisdb->push(data.ref_as<neb::host const>());
        ++_actual_query;
        break;
      case 14:
        // Host status
        _redisdb->push(data.ref_as<neb::host_status const>());
        ++_actual_query;
        break;
      case 23:
        // Service
        _redisdb->push(data.ref_as<neb::service const>());
        ++_actual_query;
        break;
      case 24:
        // Service status
        _redisdb->push(data.ref_as<neb::host_status const>());
        ++_actual_query;
        break;
      default:
        throw (exceptions::msg() << "redis: Enable to treat event of "
          << "category " << cat
          << ", element " << elem);
    }
  }
  if (_actual_query >= _queries_per_transaction) {
    logging::debug(logging::medium)
      << "redis: commiting " << _actual_query << " queries";
    int retval(_pending_queries);
    _pending_queries = 0;
    _actual_query = 0;
    _redisdb->flush();
    return retval;
  }

  return 0;
}
