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

#ifndef CCB_BAM_AVAILABILITY_BUILDER_HH
#  define CCB_BAM_AVAILABILITY_BUILDER_HH

#  include <string>
#  include <memory>
#  include <map>
#  include <set>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/bam/time/timeperiod.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class availability_builder availability_builder.hh "com/centreon/broker/bam/availability_builder.hh"
   *  @brief Availability builder
   *
   */
  class          availability_builder {
  public:
                 availability_builder(
                       time_t ending_point,
                       time_t starting_point = 0);
                 ~availability_builder();
                 availability_builder(availability_builder const& other);
    availability_builder&
                operator=(availability_builder const& other);

    void        add_event(short status,
                          time_t start, time_t end,
                          bool was_in_downtime,
                          time::timeperiod::ptr const &tp);

    int         get_available() const;
    int         get_unavailable() const;
    int         get_degraded() const;
    int         get_unknown() const;
    int         get_downtime() const;
    int         get_unavailable_opened() const;
    int         get_degraded_opened() const;
    int         get_unknown_opened() const;
    int         get_downtime_opened() const;

    void        set_timeperiod_is_default(bool val);
    bool        get_timeperiod_is_default() const;

  private:
    time_t      _start;
    time_t      _end;
    int         _available;
    int         _unavailable;
    int         _degraded;
    int         _unknown;
    int         _downtime;
    int         _alert_unavailable_opened;
    int         _alert_degraded_opened;
    int         _alert_unknown_opened;
    int         _alert_downtime_opened;

    bool        _timeperiods_is_default;
  };
}

CCB_END()

#endif // !CCB_BAM_AVAILABILITY_BUILDER_HH
