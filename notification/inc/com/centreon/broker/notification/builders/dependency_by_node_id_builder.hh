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

#ifndef CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BY_NODE_ID_BUILDER_HH
#define CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BY_NODE_ID_BUILDER_HH

#include <QHash>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/builders/dependency_builder.hh"
#include "com/centreon/broker/notification/objects/dependency.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class dependency_by_node_id_builder dependency_by_node_id_builder.hh
 * "com/centreon/broker/notification/builders/dependency_by_node_id_builder.hh"
 *  @brief Dependency builder by node id.
 *
 *  This class build a map of dependencies by their node id.
 */
class dependency_by_node_id_builder : public dependency_builder {
 public:
  dependency_by_node_id_builder(
      QMultiHash<objects::node_id, objects::dependency::ptr>& by_child_id,
      QMultiHash<objects::node_id, objects::dependency::ptr>& by_parent_id);

  virtual void add_dependency(uint32_t id, objects::dependency::ptr d);
  virtual void dependency_node_id_parent_relation(uint32_t dep_id,
                                                  objects::node_id id);
  virtual void dependency_node_id_child_relation(uint32_t dep_id,
                                                 objects::node_id id);

 private:
  dependency_by_node_id_builder();

  QHash<uint32_t, objects::dependency::ptr> _cache;
  QMultiHash<objects::node_id, objects::dependency::ptr>& _table_child_id;
  QMultiHash<objects::node_id, objects::dependency::ptr>& _table_parent_id;
};

}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_BUILDERS_DEPENDENCY_BY_NODE_ID_BUILDER_HH
