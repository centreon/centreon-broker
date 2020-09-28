/*
** Copyright 2014-2015 Centreon
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

#include "com/centreon/broker/bam/event_cache_visitor.hh"

#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
event_cache_visitor::event_cache_visitor() : io::stream("event_cache_visitor") {}

/**
 *  Destructor.
 */
event_cache_visitor::~event_cache_visitor() {}

/**
 *  Commit all the event cache to another stream.
 *
 *  @param[out] to  The stream to commit to.
 */
void event_cache_visitor::commit_to(io::stream& to) {
  for (auto const& o : _others)
    to.write(o);
  for (auto const& ba : _ba_events)
    to.write(ba);
  for (auto const& kpi : _kpi_events)
    to.write(kpi);
  _others.clear();
  _ba_events.clear();
  _kpi_events.clear();
}

/**
 *  Read an event from the stream.
 *
 *  @param[out] d         Cleared.
 *  @param[in]  deadline  Unused.
 *
 *  @return This method will throw.
 */
bool event_cache_visitor::read(std::shared_ptr<io::data>& d, time_t deadline) {
  (void)deadline;
  d.reset();
  return true;
}

/**
 *  Write an event to the stream.
 *
 *  @param[in] d  The event writen.
 *
 *  @return       Number of event acknowledged.
 */
int event_cache_visitor::write(std::shared_ptr<io::data> const& d) {
  if (!validate(d, get_name()))
    return 1;

  if (d->type() ==
      io::events::data_type<io::events::bam, bam::de_ba_event>::value)
    _ba_events.push_back(d);
  else if (d->type() ==
           io::events::data_type<io::events::bam, bam::de_kpi_event>::value)
    _kpi_events.push_back(d);
  else
    _others.push_back(d);

  return 1;
}
