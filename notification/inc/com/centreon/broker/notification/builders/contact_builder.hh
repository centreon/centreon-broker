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

#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"

CCB_BEGIN()

namespace       notification {

  class         contact_builder {
  public:
    virtual ~contact_builder() {}

    virtual void add_contact(unsigned int id,
                             shared_ptr<contact> con) = 0;
    virtual void connect_contactgroup_contact(unsigned int contact_id,
                                              unsigned int contactgroup_id) = 0;
    virtual void connect_contact_hostcommand(unsigned int contact_id,
                                             unsigned int command_id) = 0;
    virtual void connect_contact_servicecommand(unsigned int contact_id,
                                                unsigned int service_id) = 0;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_CONTACT_BUILDER_HH
