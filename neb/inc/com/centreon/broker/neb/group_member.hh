/*
** Copyright 2009-2011 Merethis
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

#ifndef CCB_NEB_GROUP_MEMBER_HH_
# define CCB_NEB_GROUP_MEMBER_HH_

# include <QString>
# include "com/centreon/broker/io/data.hh"

namespace               com {
  namespace             centreon {
    namespace           broker {
      namespace         neb {
        /**
         *  @class group_member group_member.h "com/centreon/broker/neb/group_member.hh"
         *  @brief Member of a group.
         *
         *  Base class defining that a member is part of a group.
         *
         *  @see host_group_member
         *  @see service_group_member
         */
        class           group_member : public io::data {
         private:
          void          _internal_copy(group_member const& gm);

         public:
          QString       group;
          unsigned int  host_id;
          unsigned int  instance_id;
                        group_member();
                        group_member(group_member const& gm);
          virtual       ~group_member();
          group_member& operator=(group_member const& gm);
        };
      }
    }
  }
}

#endif /* !CCB_NEB_GROUP_MEMBER_HH_ */
