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

#include "com/centreon/broker/notification/builders/composed_contact_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_contact_builder::composed_contact_builder() {}

/**
 *  Add a contact to the builder.
 *
 *  @param[in] id The id of the contact.
 *  @param[in] con The contact.
 */
void composed_contact_builder::add_contact(unsigned int id, contact::ptr con) {
  for (composed_builder<contact_builder>::iterator it(begin()), end_it(end());
       it != end_it; ++it)
    (*it)->add_contact(id, con);
}

/**
 *  Add a contact info to the builder.
 *
 *  @param[in] contact_id  The id of the contact.
 *  @param[in] key         The key of the contact info.
 *  @param[in] value       The value of the contact info.
 */
void composed_contact_builder::add_contact_info(unsigned int contact_id,
                                                std::string const& key,
                                                std::string const& value) {
  for (composed_builder<contact_builder>::iterator it(begin()), end_it(end());
       it != end_it; ++it)
    (*it)->add_contact_info(contact_id, key, value);
}
