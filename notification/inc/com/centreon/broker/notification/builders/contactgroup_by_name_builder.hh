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
   *  @class contactgroup_by_name_builder contactgroup_by_name_builder.hh "com/centreon/broker/notification/builders/contactgroup_by_name_builder.hh"
   *  @brief Contactgroup builder by name.
   *
   *  This class build a map of contactgroups by their name.
   */
  class          contactgroup_by_name_builder : public contactgroup_builder {
  public:
                 contactgroup_by_name_builder(
                   QHash<std::string, objects::contactgroup::ptr>& map);
    /**
     *  Add a contactgroup to the builder.
     *
     *  @param[in] id   The contactgroup id.
     *  @param[in] ndg  The contactgroup.
     */
    virtual void add_contactgroup(
                   unsigned int id,
                   objects::contactgroup::ptr ctg);
  private:
    QHash<std::string, objects::contactgroup::ptr>& _map;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_CONTACTGROUP_BY_NAME_BUILDER_HH
