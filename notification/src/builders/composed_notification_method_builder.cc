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

#include "com/centreon/broker/notification/builders/composed_notification_method_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_notification_method_builder::composed_notification_method_builder() {}

/**
 *  Add a notification method to the builder.
 *
 *  @param[in] method_id  The id of the notification method.
 *  @param[in] method     The method.
 */
void composed_notification_method_builder::add_notification_method(
    unsigned int method_id,
    notification_method::ptr method) {
  for (composed_builder<notification_method_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_notification_method(method_id, method);
}
