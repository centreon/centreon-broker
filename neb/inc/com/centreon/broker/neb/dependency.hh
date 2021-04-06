/*
** Copyright 2009-2013,2015 Centreon
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

#ifndef CCB_NEB_DEPENDENCY_HH
#define CCB_NEB_DEPENDENCY_HH

#include <string>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class dependency dependency.hh "com/centreon/broker/neb/dependency.hh"
 *  @brief Dependency relationship.
 *
 *  Defines a dependency between two objects.
 *
 *  @see host_dependency
 *  @see service_dependency
 */
class dependency : public io::data {
 public:
  dependency() = delete;
  dependency(uint32_t type);
  dependency(dependency const& dep);
  virtual ~dependency();
  dependency& operator=(dependency const& dep);

  std::string dependency_period;
  uint32_t dependent_host_id;
  bool enabled;
  std::string execution_failure_options;
  uint32_t host_id;
  bool inherits_parent;
  std::string notification_failure_options;

 private:
  void _internal_copy(dependency const& dep);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_DEPENDENCY_HH
