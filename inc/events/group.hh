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
*/

#ifndef CCB_EVENTS_GROUP_HH_
# define CCB_EVENTS_GROUP_HH_

# include <QString>
# include "events/event.hh"

namespace         com {
  namespace       centreon {
    namespace     broker {
      namespace   events {
        /**
         *  @class group group.hh "events/group.hh"
         *  @brief Base of host and service group classes.
         *
         *  The scheduling engine handles group. This can be service
         *  groups or host groups for example.
         *
         *  @see host_group
         *  @see service_group
         */
        class     group : public event {
         private:
          void    _internal_copy(group const& g);

         public:
          QString action_url;
          QString alias;
          int     instance_id;
          QString name;
          QString notes;
          QString notes_url;
                  group();
                  group(group const& g);
          virtual ~group();
          group&  operator=(group const& g);
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_GROUP_HH_ */
