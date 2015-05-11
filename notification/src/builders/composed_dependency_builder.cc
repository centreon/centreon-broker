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

#include "com/centreon/broker/notification/builders/composed_dependency_builder.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor
 */
composed_dependency_builder::composed_dependency_builder() {}

void composed_dependency_builder::add_dependency(
                                    unsigned int id,
                                    dependency::ptr d) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->add_dependency(id, d);
}

void composed_dependency_builder::dependency_node_id_parent_relation(
                                    unsigned int dep_id,
                                    node_id id) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->dependency_node_id_parent_relation(dep_id, id);
}

void composed_dependency_builder::dependency_node_id_child_relation(
                                    unsigned int dep_id,
                                    node_id id) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->dependency_node_id_child_relation(dep_id, id);
}

void composed_dependency_builder::set_notification_failure_options(
                                    unsigned int dep_id,
                                    std::string const& line) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->set_notification_failure_options(dep_id, line);
}

void composed_dependency_builder::set_execution_failure_options(
                                    unsigned int dep_id,
                                    std::string const& line) {
  for (composed_builder<dependency_builder>::iterator it(begin()),
                                                      it_end(end());
       it != it_end;
       ++it)
    (*it)->set_execution_failure_options(dep_id, line);
}
