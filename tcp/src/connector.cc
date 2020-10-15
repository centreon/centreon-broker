/*
 * Copyright 2011 - 2020 Centreon (https://www.centreon.com/)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * For more information : contact@centreon.com
 *
 */

#include "com/centreon/broker/tcp/connector.hh"

#include <fmt/format.h>

#include <memory>
#include <sstream>

#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tcp/stream.hh"
#include "com/centreon/broker/tcp/tcp_async.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**
 * @brief Constructor of the connector that will connect to the given host at
 * the given port. read_timeout is a duration in seconds or -1 if no limit.
 *
 * @param host The host to connect to.
 * @param port The port used for the connection.
 * @param read_timeout The read timeout in seconds or -1 if no duration.
 */
connector::connector(const std::string& host,
                     uint16_t port,
                     int32_t read_timeout)
    : io::endpoint(false),
      _host(host),
      _port(port),
      _read_timeout(read_timeout),
      _is_ready_count(0),
      _is_ready_now(0) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 * @brief Connect to the remote host.
 *
 * @return The TCP connection object.
 */
std::shared_ptr<io::stream> connector::open() {
  // Launch connection process.
  log_v2::tcp()->info("TCP: connecting to {}:{}", _host, _port);
  try {
    std::shared_ptr<stream> retval =
        std::make_shared<stream>(_host, _port, _read_timeout);
    _is_ready_count = 0;
    return retval;
  } catch (const std::exception& e) {
    if (_is_ready_count < 30)
      _is_ready_count++;
    log_v2::tcp()->debug("Unable to establish the connection to {}:{} (attempt {}): {}",
                         _host, _port, _is_ready_count, e.what());
    return std::shared_ptr<stream>();
  }
}

/**
 * @brief Return true when it is time to attempt a new connection. The idea is
 * to increase the duration between two calls each time this function is called
 * without connection between. So if now server is available, we should not
 * try to connect too often, but if the connection failed one time, it should
 * be fast to connect again.
 *
 * @return a boolean.
 */
bool connector::is_ready() const {
  time_t now;
  std::time(&now);
  if (now - _is_ready_now > (1 << _is_ready_count)) {
    _is_ready_now = now;
    return true;
  }
  return false;
}
