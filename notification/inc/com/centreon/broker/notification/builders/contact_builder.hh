/*
** Copyright 2011-2014 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class contact_builder contact_builder.hh "com/centreon/broker/notification/builders/contact_builder.hh"
   *  @brief Contact builder interface.
   *
   *  This interface define what methods contact builders need to implement.
   */
  class           contact_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual       ~contact_builder() {}

    /**
     *  Add a contact to the builder.
     *
     *  @param[in] id The id of the contact.
     *  @param[in] con The contact.
     */
    virtual void  add_contact(
                    unsigned int id,
                    objects::contact::ptr con) {
      (void)id;
      (void)con;
    }

    /**
     *  Add a contact param to the builder.
     *
     *  @param[in] contact_id  The id of the contact.
     *  @param[in] key         The key (name) of the contact param.
     *  @param[in] value       The value of the contact param.
     */
    virtual void  add_contact_param(
                    unsigned int contact_id,
                    std::string const& key,
                    std::string const& value) {
      (void)contact_id;
      (void)key;
      (void)value;
    }
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH
