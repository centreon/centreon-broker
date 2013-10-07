/*
** Copyright 2009-2012 Merethis
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

#ifndef CCB_NEB_HOST_GROUP_MEMBER_HH
#  define CCB_NEB_HOST_GROUP_MEMBER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/group_member.hh"

CCB_BEGIN()

namespace              neb {
  /**
   *  @class host_group_member host_group_member.hh "com/centreon/broker/neb/host_group_member.hh"
   *  @brief Member of a host group.
   *
   *  Base class defining that an host is part of a host group.
   *
   *  @see host
   *  @see host_group
   */
  class                host_group_member : public group_member {
  public:
                       host_group_member();
                       host_group_member(host_group_member const& hgm);
    virtual            ~host_group_member();
    host_group_member& operator=(host_group_member const& hgm);
    unsigned int       type() const;
  };
}

CCB_END()

#endif // !CCB_NEB_HOST_GROUP_MEMBER_HH
