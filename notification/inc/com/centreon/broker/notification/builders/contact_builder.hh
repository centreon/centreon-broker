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

#ifndef CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class contact_builder contact_builder.hh
 * "com/centreon/broker/notification/builders/contact_builder.hh"
 *  @brief Contact builder interface.
 *
 *  This interface define what methods contact builders need to implement.
 */
class contact_builder {
 public:
  /**
   *  Virtual destructor.
   */
  virtual ~contact_builder() {}

  /**
   *  Add a contact to the builder.
   *
   *  @param[in] id The id of the contact.
   *  @param[in] con The contact.
   */
  virtual void add_contact(uint32_t id, objects::contact::ptr con) {
    (void)id;
    (void)con;
  }

  /**
   *  Add a contact info to the builder.
   *
   *  @param[in] contact_id  The id of the contact.
   *  @param[in] key         The key of the contact info.
   *  @param[in] value       The value of the contact info.
   */
  virtual void add_contact_info(uint32_t contact_id,
                                std::string const& key,
                                std::string const& value) {
    (void)contact_id;
    (void)key;
    (void)value;
  }
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH
