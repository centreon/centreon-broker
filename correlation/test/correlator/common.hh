/*
** Copyright 2011-2012 Merethis
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

#ifndef PARSER_COMMON_HH
#  define PARSER_COMMON_HH

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
       unsigned int service_id,
       time_t start_time);
void add_issue_parent(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       unsigned int child_host_id,
       unsigned int child_service_id,
       time_t child_start_time,
       time_t end_time,
       unsigned int parent_host_id,
       unsigned int parent_service_id,
       time_t parent_start_time,
       time_t start_time);
void add_state_host(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t ack_time,
       int current_state,
       time_t end_time,
       unsigned int host_id,
       bool in_downtime,
       time_t start_time);
void add_state_service(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t ack_time,
       int current_state,
       time_t end_time,
       unsigned int host_id,
       bool in_downtime,
       unsigned int service_id,
       time_t start_time);
bool check_content(
       com::centreon::broker::io::stream& s,
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> > const& content);

#endif // !PARSER_COMMON_HH
