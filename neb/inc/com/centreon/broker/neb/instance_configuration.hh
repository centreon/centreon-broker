/*
** Copyright 2016 Centreon
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

#ifndef CCB_NEB_INSTANCE_CONFIGURATION_HH
#define CCB_NEB_INSTANCE_CONFIGURATION_HH

#include <string>

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class instance_configuration instance_configuration.hh
 * "com/centreon/broker/neb/instance_configuration.hh"
 *  @brief Information about the instance configuration.
 *
 *  An event of this class is sent when centreon engine has finished
 *  reloading its configuration, either successfully or not.
 */
class instance_configuration : public io::data {
 public:
  instance_configuration();
  instance_configuration(instance_configuration const& i);
  ~instance_configuration();
  instance_configuration& operator=(instance_configuration const& i);
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::neb,
                                 neb::de_instance_configuration>::value;
  }

  bool loaded;
  uint32_t poller_id;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(instance_configuration const& i);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_INSTANCE_CONFIGURATION_HH
