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

#ifndef CCB_NEB_INSTANCE_HH
#define CCB_NEB_INSTANCE_HH

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
 *  @class instance instance.hh "com/centreon/broker/neb/instance.hh"
 *  @brief Information about Nagios process.
 *
 *  This class holds information about a Nagios process, like whether
 *  it is running or not, in daemon mode or not, ...
 */
class instance : public io::data {
 public:
  instance();
  instance(instance const& other);
  ~instance();
  instance& operator=(instance const& other);
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::neb, neb::de_instance>::value;
  }

  std::string engine;
  bool is_running;
  std::string name;
  uint32_t pid;
  uint32_t poller_id;
  timestamp program_end;
  timestamp program_start;
  std::string version;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(instance const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_INSTANCE_HH
