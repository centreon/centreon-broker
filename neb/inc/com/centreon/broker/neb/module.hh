/*
** Copyright 2009-2012 Centreon
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

#ifndef CCB_NEB_MODULE_HH
#define CCB_NEB_MODULE_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class module module.hh "com/centreon/broker/neb/module.hh"
 *  @brief Represents a module loaded in a Nagios instance.
 *
 *  The scheduling engine supports modules that extend its
 *  original features. This class describes such modules.
 */
class module : public io::data {
 public:
  module();
  module(module const& other);
  ~module();
  module& operator=(module const& other);
  uint32_t type() const;
  static uint32_t static_type();

  std::string args;
  bool enabled;
  std::string filename;
  bool loaded;
  uint32_t poller_id;
  bool should_be_loaded;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(module const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_MODULE_HH
