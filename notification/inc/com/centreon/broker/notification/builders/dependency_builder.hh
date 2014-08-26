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
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace       notification {

  class           dependency_builder {
  public:
    virtual       ~dependency_builder() {}

    virtual void  add_dependency(unsigned int id,
                                 shared_ptr<dependency> d) {}
    virtual void  dependency_node_id_parent_relation(unsigned int dep_id,
                                                     node_id id) {}
    virtual void  dependency_node_id_child_relation(unsigned int dep_id,
                                                    node_id id) {}
    virtual void  dependency_hostgroup_parent_relation(
           unsigned int dep_id,
           unsigned int hostgroup_id) {}
    virtual void  dependency_hostgroup_child_relation(
           unsigned int dep_id,
           unsigned int hostgroup_id) {}
    virtual void  dependency_servicegroup_child_relation(
           unsigned int dep_id,
           unsigned int servicegroup_id) {}
    virtual void  dependency_servicegroup_parent_relation(
           unsigned int dep_id,
           unsigned int servicegroup_id) {}

    virtual void  set_notification_failure_options(unsigned int dep_id,
                                                   std::string const& line) {}
    virtual void  set_execution_failure_options(unsigned int dep_id,
                                                std::string const& line) {}
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH
