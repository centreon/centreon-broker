/*
** Copyright 2014-2015 Centreon
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

#ifndef CCB_BAM_SERVICE_LISTENER_HH
#define CCB_BAM_SERVICE_LISTENER_HH

#include <memory>

#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

// Forward declarations.
namespace neb {
class acknowledgement;
class downtime;
class service_status;
}  // namespace neb

namespace bam {
/**
 *  @class service_listener service_listener.hh
 * "com/centreon/broker/bam/service_listener.hh"
 *  @brief Listen to service state change.
 *
 *  This interface is used by classes wishing to listen to service
 *  state change.
 */
class service_listener {
 public:
  service_listener();
  service_listener(service_listener const& other);
  virtual ~service_listener();
  service_listener& operator=(service_listener const& other);
  virtual void service_update(
      std::shared_ptr<neb::service_status> const& status,
      io::stream* visitor = NULL);
  virtual void service_update(std::shared_ptr<neb::acknowledgement> const& ack,
                              io::stream* visitor = NULL);
  virtual void service_update(std::shared_ptr<neb::downtime> const& dt,
                              io::stream* visitor = NULL);
};
}  // namespace bam

CCB_END()

#endif  // !CCB_BAM_SERVICE_LISTENER_HH
