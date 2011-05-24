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

#ifndef CCB_EVENTS_DEPENDENCY_HH_
# define CCB_EVENTS_DEPENDENCY_HH_

# include <QString>
# include "events/event.hh"

namespace             com {
  namespace           centreon {
    namespace         broker {
      namespace       events {
        /**
         *  @class dependency dependency.hh "events/dependency.hh"
         *  @brief Dependency relationship.
         *
         *  Defines a dependency between two objects.
         *
         *  @see host_dependency
         *  @see service_dependency
         */
        class         dependency : public event {
         private:
          void        _internal_copy(dependency const& dep);

         public:
          QString     dependency_period;
          int         dependent_host_id;
          QString     execution_failure_options;
          int         host_id;
          bool        inherits_parent;
          QString     notification_failure_options;
                      dependency();
                      dependency(dependency const& dep);
          virtual     ~dependency();
          dependency& operator=(dependency const& dep);
        };
      }
    }
  }
}

#endif /* !CCB_EVENTS_DEPENDENCY_HH_ */
