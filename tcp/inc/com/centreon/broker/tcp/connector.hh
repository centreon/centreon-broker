/*
** Copyright 2011-2013, 2020-2021 Centreon
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

#ifndef CCB_TCP_CONNECTOR_HH
#define CCB_TCP_CONNECTOR_HH

#include <asio.hpp>

#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tcp {
/**
 *  @class connector connector.hh "com/centreon/broker/tcp/connector.hh"
 *  @brief TCP connector.
 *
 *  Connect to some remote TCP host.
 */
class connector : public io::endpoint {
  const std::string _host;
  const uint16_t _port;
  const int32_t _read_timeout;

  /* How many consecutive calls to is_ready() */
  mutable int16_t _is_ready_count;
  /* The time of the last call to is_ready() */
  mutable std::time_t _is_ready_now;

 public:
  connector(const std::string& host, uint16_t port, int32_t read_timeout);
  ~connector();

  connector& operator=(const connector&) = delete;
  connector(const connector&) = delete;

  std::unique_ptr<io::stream> open() override;
  bool is_ready() const override;
};
}  // namespace tcp

CCB_END()

#endif  // !CCB_TCP_CONNECTOR_HH
