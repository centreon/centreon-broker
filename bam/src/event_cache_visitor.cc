/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/event_cache_visitor.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
event_cache_visitor::event_cache_visitor() {}

/**
 *  Flags ignored.
 *
 *  @param[in] in  Ignored.
 *  @param[in] out Ignored.
 */
void event_cache_visitor::process(bool in, bool out) {}

/**
 *  Read an event from the stream.
 *
 *  @param[out] d  The event read. Ignored.
 */
void event_cache_visitor::read(misc::shared_ptr<io::data>& d) {
  d.clear();
}

/**
 *  Write an event to the stream.
 *
 *  @param[in] d  The event writen.
 *
 *  @return       Number of event acknowledged.
 */
unsigned int event_cache_visitor::write(misc::shared_ptr<io::data> const& d) {
  if (d.isNull())
    return (1);

  if (d->type()
      == io::events::data_type<io::events::bam,
                               bam::de_ba_event>::value)
    _ba_events.push_back(d);
  else if (d->type()
           == io::events::data_type<io::events::bam,
                                    bam::de_kpi_event>::value)
    _kpi_events.push_back(d);
  else
    _others.push_back(d);

  return (1);
}

/**
 *  Commit all the event cache to another stream.
 *
 *  @param[out] to  The stream to commit to.
 */
void event_cache_visitor::commit_to(io::stream& to) {
  for (std::vector<misc::shared_ptr<io::data> >::const_iterator
         it(_others.begin()),
         end(_others.end());
       it != end;
       ++it)
    to.write(*it);
  for (std::vector<misc::shared_ptr<io::data> >::const_iterator
         it(_ba_events.begin()),
         end(_ba_events.end());
       it != end;
       ++it)
    to.write(*it);
  for (std::vector<misc::shared_ptr<io::data> >::const_iterator
         it(_kpi_events.begin()),
         end(_kpi_events.end());
       it != end;
       ++it)
    to.write(*it);
  _others.clear();
  _ba_events.clear();
  _kpi_events.clear();
}
