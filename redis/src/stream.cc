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
  : _redisdb(new redisdb(address, port, password)) {}

/**
 *  Read from the connector.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Timeout.
 *
 *  @return This method will throw.
 */
bool stream::read(misc::shared_ptr<io::data>& d, time_t deadline) {
  return false;
}

/**
 *  Write an event.
 *
 *  @param[in] data Event pointer.
 *
 *  @return Number of events acknowledged.
 */
int stream::write(misc::shared_ptr<io::data> const& data) {
  // Take this event into account.
  if (!validate(data, "redis"))
    return 0;

  unsigned int type(data->type());
  unsigned short cat(io::events::category_of_type(type));
  unsigned short elem(io::events::element_of_type(type));
  QByteArray res;
  // Process event.
  if (type == instance_broadcast::static_type()) {
    _redisdb->push(data.ref_as<instance_broadcast const>());
  }
  else if (cat == io::events::neb) {
    switch (elem) {
      case 3:
        // Custom variable
        res = _redisdb->push(data.ref_as<neb::custom_variable const>());
        break;
      case 11:
        // Host group member
        res = _redisdb->push(
                data.ref_as<neb::host_group_member const>()).toByteArray();
        break;
      case 12:
        // Host
        res = _redisdb->push(data.ref_as<neb::host const>()).toByteArray();
        break;
      case 14:
        // Host status
        res = _redisdb->push(data.ref_as<neb::host_status const>()).toByteArray();
        break;
      case 15:
        // Instance
        res = _redisdb->push(data.ref_as<neb::instance const>());
        break;
      case 22:
        // Service group member
        res = _redisdb->push(data.ref_as<neb::service_group_member const>());
        break;
      case 23:
        // Service
        res = _redisdb->push(data.ref_as<neb::service const>()).toByteArray();
        break;
      case 24:
        // Service status
        res = _redisdb->push(
            data.ref_as<neb::service_status const>()).toByteArray();
        break;
      default:
        logging::info(logging::low) << "redis: Unable to treat event of "
          << "category " << cat
          << ", element " << elem;
    }
  }

  logging::error(logging::high)
    << "redis: RES = " << res.constData();

  if (res != "+OK")
    logging::error(logging::medium)
      << "redis: Unable to write event on redis server "
      << _redisdb->get_address() << ":" << _redisdb->get_port() << ".";

  return 1;
}
