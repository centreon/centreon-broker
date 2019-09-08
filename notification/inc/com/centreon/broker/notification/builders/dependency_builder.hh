/*
** Copyright 2011-2014 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/dependency.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class dependency_builder dependency_builder.hh
 * "com/centreon/broker/notification/builders/dependency_builder.hh"
 *  @brief Dependency builder interface.
 *
 *  This interface define what methods dependency builders need to implement.
 */
class dependency_builder {
 public:
  /**
   *  Virtual destructor.
   */
  virtual ~dependency_builder() {}

  /**
   *  Add a dependency to the builder.
   *
   *  @param[in] id The id of the dependency.
   *  @param[in] d The dependency.
   */
  virtual void add_dependency(unsigned int id, objects::dependency::ptr d) {
    (void)id;
    (void)d;
  }
  virtual void dependency_node_id_parent_relation(unsigned int dep_id,
                                                  objects::node_id id) {
    (void)dep_id;
    (void)id;
  }
  virtual void dependency_node_id_child_relation(unsigned int dep_id,
                                                 objects::node_id id) {
    (void)dep_id;
    (void)id;
  }

  virtual void set_notification_failure_options(unsigned int dep_id,
                                                std::string const& line) {
    (void)dep_id;
    (void)line;
  }
  virtual void set_execution_failure_options(unsigned int dep_id,
                                             std::string const& line) {
    (void)dep_id;
    (void)line;
  }
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BUILDER_HH
