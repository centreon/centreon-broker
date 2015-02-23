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

#ifndef CCB_BAM_BA_DURATION_EVENT_HH
#  define CCB_BAM_BA_DURATION_EVENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class ba_duration_event ba_duration_event.hh "com/centreon/broker/bam/ba_duration_event.hh"
   *  @brief Ba duration event
   *
   */
  class                 ba_duration_event : public io::data {
  public:
                        ba_duration_event();
                        ba_duration_event(ba_duration_event const& other);
                        ~ba_duration_event();
    ba_duration_event&  operator=(ba_duration_event const& other);
    bool                operator==(ba_duration_event const& other) const;
    unsigned int        type() const;
    static unsigned int static_type();

    unsigned int        ba_id;
    timestamp           real_start_time;
    timestamp           end_time;
    timestamp           start_time;
    unsigned int        duration;
    unsigned int        sla_duration;
    unsigned int        timeperiod_id;
    bool                timeperiod_is_default;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void                _internal_copy(ba_duration_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_BA_DURATION_EVENT_HH
