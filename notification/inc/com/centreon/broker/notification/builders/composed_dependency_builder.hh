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

#ifndef CCB_NOTIFICATION_BUILDERS_COMPOSED_DEPENDENCY_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_COMPOSED_DEPENDENCY_BUILDER_HH

#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/composed_builder.hh"
#include "com/centreon/broker/notification/builders/dependency_builder.hh"
#include "com/centreon/broker/notification/objects/dependency.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class composed_dependency_builder composed_dependency_builder.hh
 * "com/centreon/broker/notification/builders/composed_dependency_builder.hh"
 *  @brief Composed dependency builder.
 *
 *  This class forward its method call to several other builders.
 */
class composed_dependency_builder
    : public composed_builder<dependency_builder> {
 public:
  composed_dependency_builder();

  virtual void add_dependency(uint32_t id, objects::dependency::ptr d);
  virtual void dependency_node_id_parent_relation(uint32_t dep_id,
                                                  objects::node_id id);
  virtual void dependency_node_id_child_relation(uint32_t dep_id,
                                                 objects::node_id id);
  virtual void set_notification_failure_options(uint32_t dep_id,
                                                std::string const& line);
  virtual void set_execution_failure_options(uint32_t dep_id,
                                             std::string const& line);
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_COMPOSED_DEPENDENCY_BUILDER_HH
