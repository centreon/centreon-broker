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

#ifndef CCB_NEB_STATUS_HH
#define CCB_NEB_STATUS_HH

#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class status status.hh "com/centreon/broker/neb/status.hh"
 *  @brief Root class of status events.
 *
 *  This is the root class of status events : host, program and
 *  service status events.
 *
 *  @see host_status
 *  @see program_status
 *  @see service_status
 */
class status : public io::data {
 public:
  status() = delete;
  status(uint32_t type);
  status(status const& s);
  virtual ~status();
  status& operator=(status const& s);

  bool event_handler_enabled;
  bool flap_detection_enabled;
  bool notifications_enabled;

 private:
  void _internal_copy(status const& s);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_STATUS_HH
