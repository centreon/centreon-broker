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

#ifndef CCB_NEB_HOST_GROUP_HH_
# define CCB_NEB_HOST_GROUP_HH_

# include "com/centreon/broker/neb/group.hh"

namespace                com {
  namespace              centreon {
    namespace            broker {
      namespace          neb {
        /**
         *  @class host_group host_group.hh "com/centreon/broker/neb/host_group.hh"
         *  @brief Represents a group of hosts.
         *
         *  A group of hosts within the scheduling engine.
         */
        class            host_group : public group {
         public:
                         host_group();
                         host_group(host_group const& hg);
                         ~host_group();
          host_group&    operator=(host_group const& hg);
          QString const& type() const;
        };
      }
    }
  }
}

#endif /* !CCB_NEB_HOST_GROUP_HH_ */
