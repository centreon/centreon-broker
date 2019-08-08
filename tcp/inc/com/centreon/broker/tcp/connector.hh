/*
** Copyright 2011-2013 Centreon
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

#include <QMutex>
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
 public:
  connector();
  connector(connector const& other);
  ~connector();
  connector& operator=(connector const& other);
  void connect_to(std::string const& host, unsigned short port);
  std::shared_ptr<io::stream> open();
  void set_read_timeout(int secs);
  void set_write_timeout(int secs);

 private:
  void _internal_copy(connector const& other);

  std::string _host;
  unsigned short _port;
  int _read_timeout;
  int _write_timeout;
  asio::io_context _io_context;
};
}  // namespace tcp

CCB_END()

#endif  // !CCB_TCP_CONNECTOR_HH
