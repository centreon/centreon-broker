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

#ifndef CCB_NEB_SERVICE_CHECK_HH
#define CCB_NEB_SERVICE_CHECK_HH

#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/check.hh"
#include "com/centreon/broker/neb/internal.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class service_check service_check.hh
 * "com/centreon/broker/neb/service_check.hh"
 *  @brief Check that has been executed on a service.
 *
 *  Once a check has been executed on a service, an object of
 *  this class is sent.
 */
class service_check : public check {
 public:
  service_check();
  service_check(service_check const& other);
  virtual ~service_check();
  service_check& operator=(service_check const& other);
  constexpr static uint32_t static_type() {
    return io::events::data_type<io::events::neb, neb::de_service_check>::value;
  }

  uint32_t service_id;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_SERVICE_CHECK_HH
