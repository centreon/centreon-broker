/*
 * Copyright 2011 - 2019-2020 Centreon (https://www.centreon.com/)
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

#include "com/centreon/broker/tcp/factory.hh"

#include <memory>
#include <string>

#include "com/centreon/broker/config/parser.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/log_v2.hh"
#include "com/centreon/broker/tcp/acceptor.hh"
#include "com/centreon/broker/tcp/connector.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::tcp;

/**
 *  Check if a configuration supports this protocol.
 *
 *  @param[in] cfg  Object configuration.
 *
 *  @return True if the configuration has this protocol.
 */
bool factory::has_endpoint(config::endpoint& cfg) const {
  return cfg.type == "ip" || cfg.type == "tcp" || cfg.type == "ipv4" ||
         cfg.type == "ipv6";
}

/**
 *  Create a new endpoint from a configuration.
 *
 *  @param[in]  cfg         Endpoint configuration.
 *  @param[out] is_acceptor Set to true if the endpoint is an acceptor.
 *  @param[in]  cache       Unused.
 *
 *  @return Endpoint matching configuration.
 */
io::endpoint* factory::new_endpoint(
    config::endpoint& cfg,
    bool& is_acceptor,
    std::shared_ptr<persistent_cache> cache) const {
  (void)cache;

  // Find host (if exists).
  std::string host;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("host")};
    if (it != cfg.params.end())
      host = it->second;
  }

  // Find port (must exist).
  uint16_t port;
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("port")};
    if (it == cfg.params.end()) {
      log_v2::tcp()->error("TCP: no 'port' defined for endpoint '{}'",
                           cfg.name);
      throw exceptions::msg() << "TCP: no 'port' defined for "
                                 "endpoint '"
                              << cfg.name << "'";
    }
    port = static_cast<uint16_t>(std::stol(it->second));
  }

  // Find TCP socket timeout options.
  int write_timeout(-1);
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("socket_write_timeout")};
    if (it != cfg.params.end())
      write_timeout = std::stoul(it->second);
  }

  int read_timeout(-1);
  {
    std::map<std::string, std::string>::const_iterator it{
        cfg.params.find("socket_read_timeout")};
    if (it != cfg.params.end())
      read_timeout = std::stoul(it->second);
  }

  // Acceptor.
  std::unique_ptr<io::endpoint> endp;
  if (host.empty()) {
    is_acceptor = true;
    std::unique_ptr<tcp::acceptor> a(
        new tcp::acceptor(port, read_timeout, write_timeout));
    endp.reset(a.release());
  }
  // Connector.
  else {
    is_acceptor = false;
    std::unique_ptr<tcp::connector> c(
        new tcp::connector(host, port, read_timeout, write_timeout));
    endp.reset(c.release());
  }
  return endp.release();
}
