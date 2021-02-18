/*
** Copyright 2013 Centreon
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

#ifndef CCB_TLS_FACTORY_HH
#define CCB_TLS_FACTORY_HH

#include "com/centreon/broker/io/factory.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace tls {
/**
 *  @class factory factory.hh "com/centreon/broker/tls/factory.hh"
 *  @brief TLS security layer factory.
 *
 *  Build TLS security objects.
 */
class factory : public io::factory {
  std::string _ca_cert;
  std::string _private_key;
  std::string _public_cert;
  std::string _tls_hostname;

 public:
  factory() = default;
  ~factory() = default;
  factory(factory const& other) = delete;
  factory& operator=(factory const& other) = delete;
  bool has_endpoint(config::endpoint& cfg, flag* flag);
  io::endpoint* new_endpoint(config::endpoint& cfg,
                             bool& is_acceptor,
                             std::shared_ptr<persistent_cache> cache =
                                 std::shared_ptr<persistent_cache>()) const;
  std::shared_ptr<io::stream> new_stream(std::shared_ptr<io::stream> to,
                                         bool is_acceptor,
                                         std::string const& proto_name);
};
}  // namespace tls

CCB_END()

#endif  // !CCB_TLS_FACTORY_HH
