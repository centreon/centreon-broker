/*
** Copyright 2011-2013 Centreon
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

#ifndef TEST_CORRELATOR_COMMON_HH
#  define TEST_CORRELATOR_COMMON_HH

#  include <ctime>
#  include <QList>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/stream.hh"

void add_engine_state(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       bool started);
void add_issue(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t ack_time,
       time_t end_time,
       unsigned int host_id,
       unsigned int instance_id,
       unsigned int service_id,
       time_t start_time);
void add_issue_parent(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       unsigned int child_host_id,
       unsigned int child_instance_id,
       unsigned int child_service_id,
       time_t child_start_time,
       time_t end_time,
       unsigned int parent_host_id,
       unsigned int parent_instance_id,
       unsigned int parent_service_id,
       time_t parent_start_time,
       time_t start_time);
void add_state_host(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t ack_time,
       int current_state,
       time_t end_time,
       unsigned int host_id,
       unsigned int instance_id,
       bool in_downtime,
       time_t start_time);
void add_state_service(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t ack_time,
       int current_state,
       time_t end_time,
       unsigned int host_id,
       unsigned int instance_id,
       bool in_downtime,
       unsigned int service_id,
       time_t start_time);
void check_content(
       com::centreon::broker::io::stream& s,
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> > const& content);

#endif // !TEST_CORRELATOR_COMMON_HH
