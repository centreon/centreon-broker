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

#ifndef CCB_NEB_SERVICE_GROUP_MEMBER_HH_
# define CCB_NEB_SERVICE_GROUP_MEMBER_HH_

# include "com/centreon/broker/neb/group_member.hh"

namespace                       com {
  namespace                     centreon {
    namespace                   broker {
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
          unsigned int          service_id;
                                service_group_member();
                                service_group_member(
                                  service_group_member const& sgm);
          virtual               ~service_group_member();
          service_group_member& operator=(service_group_member const& sgm);
          QString const&        type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_SERVICE_GROUP_MEMBER_HH_ */
