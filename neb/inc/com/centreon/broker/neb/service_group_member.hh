/*
** Copyright 2009-2012,2015 Merethis
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

#ifndef CCB_NEB_SERVICE_GROUP_MEMBER_HH
#  define CCB_NEB_SERVICE_GROUP_MEMBER_HH

#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/group_member.hh"

CCB_BEGIN()

namespace                 neb {
  /**
   *  @class service_group_member service_group_member.hh "com/centreon/broker/neb/service_group_member.hh"
   *  @brief Member of a service group.
   *
   *  Base class defining that a service is part of a service
   *  group.
   *
   *  @see service
   *  @see service_group
   */
  class                   service_group_member : public group_member {
  public:
                          service_group_member();
                          service_group_member(
                            service_group_member const& other);
    virtual               ~service_group_member();
    service_group_member& operator=(service_group_member const& other);
    unsigned int          type() const;
    static unsigned int   static_type();

    unsigned int          service_id;

    static mapping::entry const
                          entries[];
    static io::event_info::event_operations const
                          operations;
  };
}

CCB_END()

#endif // !CCB_NEB_SERVICE_GROUP_MEMBER_HH
