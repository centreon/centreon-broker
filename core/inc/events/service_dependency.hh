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

#ifndef CCB_EVENTS_SERVICE_DEPENDENCY_HH_
# define CCB_EVENTS_SERVICE_DEPENDENCY_HH_

# include "events/dependency.hh"

namespace                     com {
  namespace                   centreon {
    namespace                 broker {
      namespace               events {
        /**
         *  @class service_dependency service_dependency.hh "events/service_dependency.hh"
         *  @brief Service dependency relationship.
         *
         *  Defines a dependency between two services.
         */
        class                 service_dependency : public dependency {
         private:
          void                _internal_copy(service_dependency const& sd);

         public:
          int                 dependent_service_id;
          int                 service_id;
                              service_dependency();
                              service_dependency(
                                service_dependency const& sd);
                              ~service_dependency();
          service_dependency& operator=(service_dependency const& sd);
          unsigned int        type() const;
        };
      }
    }
  }
}

#endif /* !EVENTS_SERVICE_DEPENDENCY_HH_ */
