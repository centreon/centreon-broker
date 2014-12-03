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

#ifndef CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BUILDER_HH

#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/contactgroup.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class contactgroup_builder contactgroup_builder.hh "com/centreon/broker/notification/builders/contactgroup_builder.hh"
   *  @brief Contactgroup builder interface.
   *
   *  This interface define what methods contactgroup builders need to implement.
   */
  class          contactgroup_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual      ~contactgroup_builder() {}

    /**
     *  Add a contactgroup to the builder.
     *
     *  @param[in] id   The contactgroup id.
     *  @param[in] ndg  The contactgroup.
     */
    virtual void add_contactgroup(
                   unsigned int id,
                   objects::contactgroup::ptr ctg) {}

    /**
     *  Add a contactgroup contact relation to the builder.
     *
     *  @param[in] contact_id       The id of the contact.
     *  @param[in] contactgroup_id  The id of the contactgroup.
     */
    virtual void add_contactgroup_contact_relation(
                   unsigned int contact_id,
                   unsigned int contactgroup_id) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BUILDER_HH
