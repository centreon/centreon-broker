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

#ifndef TEST_NEB_NODE_EVENTS_COMMON_HH
#  define TEST_NEB_NODE_EVENTS_COMMON_HH

#  include <vector>
#  include <ctime>
#  include <QList>
#  include <QString>
#  include <string>
#  include "com/centreon/broker/multiplexing/hooker.hh"
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/neb/downtime.hh"
#  include "com/centreon/broker/neb/acknowledgement.hh"
#  include "com/centreon/broker/timestamp.hh"

QString format_command(
          QString const& command,
          time_t when = 0,
          time_t when2 = 0);

void add_downtime(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t start_time,
       time_t end_time,
       time_t duration,
       bool fixed,
       unsigned int host_id,
       unsigned int service_id,
       unsigned int internal_id,
       short downtime_type,
       time_t real_start_time,
       time_t real_end_time);

void add_acknowledgement(
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >& content,
       time_t entry_time,
       time_t deletion_time,
       unsigned int host_id,
       unsigned int service_id,
       bool is_sticky,
       short state);

void check_content(
       com::centreon::broker::io::stream& s,
       QList<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> > const& content);

class test_stream : public com::centreon::broker::multiplexing::hooker {
public:
  virtual bool             read(
    com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data>& d,
    time_t deadline);
  virtual int              write(
    com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> const& d);
  std::vector<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> > const&
                           get_events() const;
  virtual void             starting();
  virtual void             stopping();
  void                     finalize();
public:
  bool                     _finalized;
  std::vector<com::centreon::broker::misc::shared_ptr<com::centreon::broker::io::data> >
                           _events;
};

#endif // !TEST_NEB_NODE_EVENTS_COMMON_HH
