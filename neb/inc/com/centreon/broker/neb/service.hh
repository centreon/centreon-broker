/*
** Copyright 2009-2013,2015 Centreon
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

#ifndef CCB_NEB_SERVICE_HH
#define CCB_NEB_SERVICE_HH

#include <ctime>
#include <string>
#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/host_service.hh"
#include "com/centreon/broker/neb/service_status.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class service service.hh "com/centreon/broker/neb/service.hh"
 *  @brief Service as handled by the scheduling engine.
 *
 *  Holds full data regarding a service.
 *
 *  @see host_service
 *  @see service_status
 */
class service : public host_service, public service_status {
 public:
  service();
  service(service_status const& ss);
  service(service const& other);
  ~service();
  service& operator=(service const& other);
  uint32_t type() const;
  static uint32_t static_type();

  bool flap_detection_on_critical;
  bool flap_detection_on_ok;
  bool flap_detection_on_unknown;
  bool flap_detection_on_warning;
  bool is_volatile;
  bool notify_on_critical;
  bool notify_on_unknown;
  bool notify_on_warning;
  bool stalk_on_critical;
  bool stalk_on_ok;
  bool stalk_on_unknown;
  bool stalk_on_warning;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(service const& other);
  void _zero_initialize();
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_SERVICE_HH
