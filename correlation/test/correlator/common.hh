/*
** Copyright 2011-2013,2015 Centreon
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
#define TEST_CORRELATOR_COMMON_HH

#include <QList>
#include <ctime>
#include <memory>
#include <vector>
#include "com/centreon/broker/io/data.hh"
#include "com/centreon/broker/io/stream.hh"
#include "com/centreon/broker/multiplexing/hooker.hh"

void add_engine_state(
    QList<std::shared_ptr<com::centreon::broker::io::data> >& content,
    bool started);
void add_issue(
    QList<std::shared_ptr<com::centreon::broker::io::data> >& content,
    time_t ack_time,
    time_t end_time,
    unsigned int host_id,
    unsigned int service_id,
    time_t start_time);
void add_issue_parent(
    QList<std::shared_ptr<com::centreon::broker::io::data> >& content,
    unsigned int child_host_id,
    unsigned int child_service_id,
    time_t child_start_time,
    time_t end_time,
    unsigned int parent_host_id,
    unsigned int parent_service_id,
    time_t parent_start_time,
    time_t start_time);
void add_state(
    QList<std::shared_ptr<com::centreon::broker::io::data> >& content,
    time_t ack_time,
    int current_state,
    time_t end_time,
    unsigned int host_id,
    bool in_downtime,
    unsigned int service_id,
    time_t start_time);
void check_content(
    com::centreon::broker::io::stream& s,
    QList<std::shared_ptr<com::centreon::broker::io::data> > const& content);

class test_stream : public com::centreon::broker::multiplexing::hooker {
 public:
  virtual bool read(std::shared_ptr<com::centreon::broker::io::data>& d,
                    time_t deadline);
  virtual int write(std::shared_ptr<com::centreon::broker::io::data> const& d);
  std::vector<std::shared_ptr<com::centreon::broker::io::data> > const&
  get_events() const;
  virtual void starting();
  virtual void stopping();
  void finalize();

 public:
  bool _finalized;
  std::vector<std::shared_ptr<com::centreon::broker::io::data> > _events;
};

#endif  // !TEST_CORRELATOR_COMMON_HH
