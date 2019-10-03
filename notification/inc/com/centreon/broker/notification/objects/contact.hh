/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_NOTIFICATION_CONTACT_HH
#define CCB_NOTIFICATION_CONTACT_HH

#include <map>
#include <string>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/defines.hh"
#include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace notification {
namespace objects {
/**
 *  @class contact contact.hh
 * "com/centreon/broker/notification/objects/contact.hh"
 *  @brief Contact object.
 *
 *  The object containing a contact.
 */
class contact {
 public:
  DECLARE_SHARED_PTR(contact);

  contact();
  contact(contact const& obj);
  contact& operator=(contact const& obj);

  uint32_t get_id() const throw();
  void set_id(uint32_t);
  std::string const& get_description() const throw();
  void set_description(std::string const& desc);

 private:
  uint32_t _id;
  std::string _description;
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_CONTACT_HH
