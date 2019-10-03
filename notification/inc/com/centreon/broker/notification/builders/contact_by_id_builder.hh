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

#ifndef CCB_NOTIFICATION_BUILDERS_CONTACT_BY_ID_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_CONTACT_BY_ID_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/contact_builder.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class contact_by_id_builder contact_by_id_builder.hh
 * "com/centreon/broker/notification/builders/contact_by_id_builder.hh"
 *  @brief Contact by id builder.
 */
class contact_by_id_builder : public contact_builder {
 public:
  contact_by_id_builder(
      QHash<uint32_t, objects::contact::ptr>& table,
      QHash<uint32_t, QHash<std::string, std::string> >& contact_infos);

  void add_contact(uint32_t id, objects::contact::ptr con);

  void add_contact_info(uint32_t contact_id,
                        std::string const& key,
                        std::string const& value);

 private:
  QHash<uint32_t, objects::contact::ptr>& _table;
  QHash<uint32_t, QHash<std::string, std::string> >& _contact_infos;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_CONTACT_BY_ID_BUILDER_HH
