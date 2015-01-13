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

#ifndef CCB_BAM_EVENT_CACHE_VISITOR_HH
#  define CCB_BAM_EVENT_CACHE_VISITOR_HH

#  include <vector>
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class event_cache_visitor event_cache_visitor.hh "com/centreon/broker/bam/event_cache_visitor.hh"
   *  @brief event_cache_visitor cache the events
   *
   *  This class caches the events and commit them to the multiplexing in
   *  this order: others, ba_events, kpi_events.
   */
  class                  event_cache_visitor : public io::stream {
  public:
                         event_cache_visitor();

    virtual void         process(
                           bool in = false,
                           bool out = true);
    virtual void         read(misc::shared_ptr<io::data>& d);
    virtual unsigned int write(misc::shared_ptr<io::data> const& d);

    void                 commit_to(io::stream& to);

  private:
                         event_cache_visitor(event_cache_visitor const&);
    event_cache_visitor& operator=(event_cache_visitor const&);

    std::vector<misc::shared_ptr<io::data> >
                         _others;
    std::vector<misc::shared_ptr<io::data> >
                         _ba_events;
    std::vector<misc::shared_ptr<io::data> >
                         _kpi_events;
  };
}

CCB_END()

#endif // !CCB_BAM_EVENT_CACHE_VISITOR_HH
