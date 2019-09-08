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

#ifndef CCB_IO_FACTORY_HH
#define CCB_IO_FACTORY_HH

#include <memory>
#include "com/centreon/broker/config/endpoint.hh"
#include "com/centreon/broker/io/endpoint.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/persistent_cache.hh"

CCB_BEGIN()

namespace io {
/**
 *  @class factory factory.hh "com/centreon/broker/io/factory.hh"
 *  @brief Endpoint factory.
 *
 *  Build endpoint according to some configuration.
 */
class factory {
 public:
  factory();
  factory(factory const& other);
  virtual ~factory();
  factory& operator=(factory const& other);
  virtual factory* clone() const = 0;
  virtual bool has_endpoint(
      com::centreon::broker::config::endpoint& cfg) const = 0;
  virtual bool has_not_endpoint(
      com::centreon::broker::config::endpoint& cfg) const;
  virtual endpoint* new_endpoint(
      com::centreon::broker::config::endpoint& cfg,
      bool& is_acceptor,
      std::shared_ptr<persistent_cache> cache =
          std::shared_ptr<persistent_cache>()) const = 0;
  virtual std::shared_ptr<stream> new_stream(std::shared_ptr<stream> substream,
                                             bool is_acceptor,
                                             std::string const& proto_name);
};
}  // namespace io

CCB_END()

#endif  // !CCB_IO_FACTORY_HH
