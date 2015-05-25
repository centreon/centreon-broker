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

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/dependency.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace         notification {
  /**
   *  @class dependency_builder dependency_builder.hh "com/centreon/broker/notification/builders/dependency_builder.hh"
   *  @brief Dependency builder interface.
   *
   *  This interface define what methods dependency builders need to implement.
   */
  class           dependency_builder {
  public:
    /**
     *  Virtual destructor.
     */
    virtual       ~dependency_builder() {}

    /**
     *  Add a dependency to the builder.
     *
     *  @param[in] id The id of the dependency.
     *  @param[in] d The dependency.
     */
    virtual void  add_dependency(
                    unsigned int id,
                    objects::dependency::ptr d) {
      (void)id;
      (void)d;
    }
    virtual void  dependency_node_id_parent_relation(
                    unsigned int dep_id,
                    objects::node_id id) {
      (void)dep_id;
      (void)id;
    }
    virtual void  dependency_node_id_child_relation(
                    unsigned int dep_id,
                    objects::node_id id) {
      (void)dep_id;
      (void)id;
    }

    virtual void  set_notification_failure_options(
                    unsigned int dep_id,
                    std::string const& line) {
      (void)dep_id;
      (void)line;
    }
    virtual void  set_execution_failure_options(
                    unsigned int dep_id,
                    std::string const& line) {
      (void)dep_id;
      (void)line;
    }
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH
