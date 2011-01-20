/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef EVENTS_GROUP_MEMBER_HH_
# define EVENTS_GROUP_MEMBER_HH_

# include <string>
# include "events/event.hh"

namespace         events {
  /**
   *  @class group_member group_member.h "events/group_member.hh"
   *  @brief Member of a group.
   *
   *  Base class defining that a member is part of a group.
   *
   *  @see host_group_member
   *  @see service_group_member
   */
  class           group_member : public event {
   private:
    void          _internal_copy(group_member const& gm);

   public:
    std::string   group;
    int           host_id;
    int           instance_id;
                  group_member();
                  group_member(group_member const& gm);
    virtual       ~group_member();
    group_member& operator=(group_member const& gm);
  };
}

#endif /* !EVENTS_GROUP_MEMBER_HH_ */
