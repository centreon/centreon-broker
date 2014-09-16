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

#ifndef CCB_BAM_INDICATOR_EVENT_HH
#  define CCB_BAM_INDICATOR_EVENT_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace             bam {
  /**
   *  @class indicator_event indicator_event.hh "com/centreon/broker/bam/indicator_event.hh"
   *  @brief Indicator event specialized by kpi and ba event.
   *
   */
  class               indicator_event : public io::data {
  public:
                      indicator_event();
                      indicator_event(indicator_event const& other);
                      ~indicator_event();
    indicator_event&  operator=(indicator_event const& other);

    unsigned int      status;
    bool              in_downtime;
    timestamp         start_time;
    unsigned int      duration;
    unsigned int      timeperiod_id;
    bool              timeperiod_is_default;

  private:
    void              _internal_copy(indicator_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_INDICATOR_EVENT_HH
