/*
** Copyright 2018 Centreon
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

#ifndef CCB_NEB_RESPONSIVE_INSTANCE_HH
#define CCB_NEB_RESPONSIVE_INSTANCE_HH

#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class responsive_instance responsive_instance.hh
 * "com/centreon/broker/neb/responsive_instance.hh"
 *  @brief Information about a missing instance.
 *
 *  An event of this class is emitted by Centreon Broker if an engine instance
 *  does not give any sign of life or if it gives some back again.
 */
class responsive_instance : public io::data {
 public:
  responsive_instance();
  responsive_instance(responsive_instance const& i);
  ~responsive_instance();
  responsive_instance& operator=(responsive_instance const& i);
  uint32_t type() const;

  uint32_t poller_id;
  bool responsive;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(responsive_instance const& i);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_RESPONSIVE_INSTANCE_HH
