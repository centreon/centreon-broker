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

#ifndef CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH
#  define CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH

#  include "com/centreon/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/dependency.hh"

CCB_BEGIN()

namespace       notification {

  class         dependency_builder {
  public:
    virtual ~dependency_builder() {}

    virtual void add_dependency(unsigned int id,
                                shared_ptr<dependency> d) = 0;
    virtual void dependency_host_child_relation(unsigned int dep_id,
                                                unsigned int child_id) = 0;
    virtual void dependency_host_parent_relation(unsigned int dep_id,
                                                 unsigned int child_id) = 0;
    virtual void dependency_service_child_relation(unsigned int dep_id,
                                                   unsigned int child_id) = 0;
    virtual void dependency_service_parent_relation(unsigned int dep_id,
                                                    unsigned int child_id) = 0;

    virtual void set_notification_failure_options(unsigned int dep_id,
                                                  std::string const& line) = 0;
    virtual void set_execution_failure_options(unsigned int dep_id,
                                               std::string const& line) = 0;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH
