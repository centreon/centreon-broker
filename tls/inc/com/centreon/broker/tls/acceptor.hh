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

#ifndef CCB_TLS_ACCEPTOR_HH
#define CCB_TLS_ACCEPTOR_HH

#include <string>

#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tls {
/**
 *  @class acceptor acceptor.hh "com/centreon/broker/tls/acceptor.hh"
 *  @brief Perform TLS verification on top of another acceptor.
 *
 *  Within the process of accepting an incoming client, the TLS
 *  acceptor class will provide encryption to the lower stream. Using
 *  this class is really simple : build the object, set some
 *  properties and call open(). Then use it just like you'd use
 *  another io::stream. Encryption will be automatically provided on
 *  the returned accepted streams.
 */
class acceptor : public io::endpoint {
  std::string _ca;
  std::string _cert;
  std::string _key;
  std::string _tls_hostname;

 public:
  acceptor(std::string const& cert = std::string(),
           std::string const& key = std::string(),
           std::string const& ca = std::string(),
           std::string const& tls_hostname = std::string());
  ~acceptor() = default;
  acceptor(acceptor const& right) = delete;
  acceptor& operator=(acceptor const&) = delete;
  std::shared_ptr<io::stream> open();
  std::shared_ptr<io::stream> open(std::shared_ptr<io::stream> lower);
};
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_ACCEPTOR_HH
