/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef CORRELATION_CORRELATOR_HH_
# define CORRELATION_CORRELATOR_HH_

# include <list>
# include <map>
# include "correlation/node.hh"
# include "events/event.hh"
# include "events/host_status.hh"
# include "events/log_entry.hh"

namespace                      correlation {
  /**
   *  @class correlator correlator.hh "correlation/correlator.hh"
   *  @brief Create event correlation.
   *
   *  This class aggregate data from multiple Nagios events and
   *  generates according issues which are used to group those events.
   */
  class                        correlator {
   private:
    static void (correlator::* _dispatch_table[])(events::event&);
    std::list<events::event*>  _events;
    std::map<int, node>        _hosts;
    std::map<std::pair<int, int>, node>
                               _services;
    void                       _correlate_host_service_status(events::event& e,
                                 bool is_host);
    void                       _correlate_host_status(events::event& e);
    void                       _correlate_log(events::event& e);
    void                       _correlate_nothing(events::event& e);
    void                       _correlate_service_status(events::event& e);
    events::issue*             _find_related_issue(node& n);
    void                       _internal_copy(correlator const& c);

   public:
                               correlator();
                               correlator(correlator const& c);
                               ~correlator();
    correlator&                operator=(correlator const& c);
    void                       event(events::event& e);
    events::event*             event();
    bool                       has_events();
    void                       load(char const* correlation_file);
  };
}

#endif /* !CORRELATION_CORRELATOR_HH_ */
