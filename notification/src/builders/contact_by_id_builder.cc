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

#include "com/centreon/broker/notification/builders/contact_by_id_builder.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 *
 *  @param[in] table  The table to fill.
 */
contact_by_id_builder::contact_by_id_builder(
    QHash<uint32_t, objects::contact::ptr>& table,
    QHash<uint32_t, QHash<std::string, std::string> >& contact_infos)
    : _table(table), _contact_infos(contact_infos) {}

/**
 *  Add a contact to the builder.
 *
 *  @param[in] id   The id of the contact.
 *  @param[in] con  The contact to add.
 */
void contact_by_id_builder::add_contact(uint32_t id,
                                        objects::contact::ptr con) {
  _table[id] = con;
}

/**
 *  Add a contact info to the builder.
 *
 *  @param[in] contact_id  The id of the contact.
 *  @param[in] key         The key of the contact info.
 *  @param[in] value       The value of the contact info.
 */
void contact_by_id_builder::add_contact_info(uint32_t contact_id,
                                             std::string const& key,
                                             std::string const& value) {
  _contact_infos[contact_id].insert(key, value);
}
