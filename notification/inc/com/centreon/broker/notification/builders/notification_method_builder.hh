/*
** Copyright 2011-2014 Centreon
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

#ifndef CCB_NOTIFICATION_BUILDERS_NOTIFICATION_METHOD_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_NOTIFICATION_METHOD_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/notification_method.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class notification_method_builder notification_method_builder.hh
 * "com/centreon/broker/notification/builders/notification_method_builder.hh"
 */
class notification_method_builder {
 public:
  virtual ~notification_method_builder() {}

  /**
   *  Add a notification method to the builder.
   *
   *  @param[in] method_id  The id of the notification method.
   *  @param[in] method     The method.
   */
  virtual void add_notification_method(
      unsigned int method_id,
      objects::notification_method::ptr method) {
    (void)method_id;
    (void)method;
  }
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_NOTIFICATION_METHOD_BUILDER_HH
