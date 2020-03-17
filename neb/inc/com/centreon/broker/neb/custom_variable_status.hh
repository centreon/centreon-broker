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

#ifndef CCB_NEB_CUSTOM_VARIABLE_STATUS_HH
#define CCB_NEB_CUSTOM_VARIABLE_STATUS_HH

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
 *  @class custom_variable_status custom_variable_status.hh
 * "com/centreon/broker/neb/custom_variable_status.hh"
 *  @brief Custom variable update.
 *
 *  The value of a custom variable has changed.
 */
class custom_variable_status : public io::data {
 public:
  custom_variable_status(uint32_t type = static_type());
  custom_variable_status(custom_variable_status const& other);
  virtual ~custom_variable_status();
  custom_variable_status& operator=(custom_variable_status const& other);
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::neb,
                                 neb::de_custom_variable_status>::value;
  }

  uint32_t host_id;
  bool modified;
  std::string name;
  uint32_t service_id;
  timestamp update_time;
  std::string value;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(custom_variable_status const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_CUSTOM_VARIABLE_STATUS_HH
