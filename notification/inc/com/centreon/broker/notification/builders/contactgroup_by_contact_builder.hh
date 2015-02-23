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

#ifndef CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BY_NAME_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BY_NAME_BUILDER_HH

#  include <string>
#  include <QHash>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/builders/contactgroup_builder.hh"

CCB_BEGIN()

namespace        notification {
  /**
   *  @class contactgroup_by_contact_builder contactgroup_by_contact_builder.hh "com/centreon/broker/notification/builders/contactgroup_by_contact_builder.hh"
   *  @brief Contactgroup builder by contact id.
   *
   *  This class build a map of contactgroups by contact id and inversely.
   */
  class          contactgroup_by_contact_builder : public contactgroup_builder {
  public:
                 contactgroup_by_contact_builder(
                   QMultiHash<
                     unsigned int,
                     objects::contactgroup::ptr>& group_by_contact,
                   QMultiHash<
                     objects::contactgroup::ptr,
                     unsigned int>& contact_by_group);
    /**
     *  Add a contactgroup to the builder.
     *
     *  @param[in] id   The contactgroup id.
     *  @param[in] ndg  The contactgroup.
     */
    virtual void add_contactgroup(
                   unsigned int id,
                   objects::contactgroup::ptr ctg);


    virtual void add_contactgroup_contact_relation(
                   unsigned int contact_id,
                   unsigned int contactgroup_id);
  private:
    QHash<unsigned int, objects::contactgroup::ptr> _cache;
    QMultiHash<unsigned int, objects::contactgroup::ptr>& _group_by_contact;
    QMultiHash<objects::contactgroup::ptr, unsigned int>& _contact_by_group;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BY_NAME_BUILDER_HH
