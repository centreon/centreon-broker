/*
** Copyright 2009-2013 Centreon
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

#ifndef CCB_TLS_CONNECTOR_HH
#define CCB_TLS_CONNECTOR_HH

#include <string>

#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tls {
/**
 *  @class connector connector.hh "com/centreon/broker/tls/connector.hh"
 *  @brief Connect to a TLS peer.
 *
 *  Use a connected TLS stream to connect to a TLS peer.
 */
class connector : public io::endpoint {
  std::string _ca;
  std::string _cert;
  std::string _key;
  std::string _tls_hostname;

 public:
  connector(std::string const& cert = std::string(),
            std::string const& key = std::string(),
            std::string const& ca = std::string(),
            std::string const& tls_hostname = std::string());
  ~connector() = default;
  connector(connector const& right) = delete;
  connector& operator=(connector const& right) = delete;
  std::shared_ptr<io::stream> open();
  std::shared_ptr<io::stream> open(std::shared_ptr<io::stream> lower);
};
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_CONNECTOR_HH
