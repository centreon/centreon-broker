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

#include "com/centreon/broker/notification/builders/composed_dependency_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_dependency_builder::composed_dependency_builder() {}

void composed_dependency_builder::add_dependency(unsigned int id,
                                                 dependency::ptr d) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->add_dependency(id, d);
}

void composed_dependency_builder::dependency_node_id_parent_relation(
    unsigned int dep_id,
    node_id id) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->dependency_node_id_parent_relation(dep_id, id);
}

void composed_dependency_builder::dependency_node_id_child_relation(
    unsigned int dep_id,
    node_id id) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->dependency_node_id_child_relation(dep_id, id);
}

void composed_dependency_builder::set_notification_failure_options(
    unsigned int dep_id,
    std::string const& line) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->set_notification_failure_options(dep_id, line);
}

void composed_dependency_builder::set_execution_failure_options(
    unsigned int dep_id,
    std::string const& line) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
       it_end(end());
       it != it_end; ++it)
    (*it)->set_execution_failure_options(dep_id, line);
}
