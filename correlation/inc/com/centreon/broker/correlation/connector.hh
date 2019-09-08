/*
** Copyright 2015 Centreon
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

#ifndef CCB_CORRELATION_CONNECTOR_HH
#define CCB_CORRELATION_CONNECTOR_HH

#include <memory>
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace correlation {
/**
 *  @class connector connector.hh "com/centreon/broker/correlation/connector.hh"
 *  @brief Open a correlation stream.
 *
 *  Generate a correlation stream that will generation correlation
 *  events (issue, issue parenting, host/service state events, ...).
 */
class connector : public io::endpoint {
 public:
  connector(std::string const& correlation_file,
            bool passive = false,
            std::shared_ptr<persistent_cache> cache =
                std::shared_ptr<persistent_cache>());
  connector(connector const& other);
  ~connector();
  connector& operator=(connector const& other);
  std::shared_ptr<io::stream> open();

 private:
  std::shared_ptr<persistent_cache> _cache;
  std::string _correlation_file;
  bool _passive;
};
}  // namespace correlation

CCB_END()

#endif  // !CCB_CORRELATION_CONNECTOR_HH
