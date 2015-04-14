/*
** Copyright 2011-2013 Merethis
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

#ifndef TEST_CORRELATOR_COMMON_HH
#  define TEST_CORRELATOR_COMMON_HH

#  include <vector>
#  include <ctime>
#  include <QList>
#  include "com/centreon/broker/multiplexing/hooker.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

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
void add_state(
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

class test_stream : public com::centreon::broker::multiplexing::hooker {
  virtual void             read(
    com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data>& d);
  virtual unsigned int     write(
    com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> const& d);
  std::vector<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> > const&
                           get_events() const;
  virtual void             starting();
  virtual void             stopping();
public:
  std::vector<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >
                           _events;
};

#endif // !TEST_CORRELATOR_COMMON_HH
