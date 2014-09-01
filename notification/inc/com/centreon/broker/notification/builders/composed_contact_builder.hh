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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_CONTACT_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_COMPOSED_CONTACT_BUILDER_HH

#  include <vector>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/builders/contact_builder.hh"
#  include "com/centreon/broker/notification/builders/composed_builder.hh"

CCB_BEGIN()

namespace       notification {

  class           composed_contact_builder : public composed_builder<contact_builder> {
  public:
                  composed_contact_builder();
    virtual       ~composed_contact_builder() {}

    virtual void  add_contact(unsigned int id,
                              contact::ptr con);
    virtual void  connect_contact_contactgroup(unsigned int contact_id,
                                               unsigned int contactgroup_id);
    virtual void  connect_contact_hostcommand(unsigned int contact_id,
                                              std::string const& command_name);
    virtual void  connect_contact_servicecommand(unsigned int contact_id,
                                                 std::string const& command_name);
    virtual void  add_contact_param(unsigned int contact_id,
                                    std::string const& key,
                                    std::string const& value);
    virtual void  connect_contact_node_id(unsigned int contact_id,
                                          node_id id);
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_COMPOSED_CONTACT_BUILDER_HH
