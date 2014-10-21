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

#ifndef CCB_BAM_KPI_EVENT_HH
#  define CCB_BAM_KPI_EVENT_HH

#  include <string>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class kpi_event kpi_event.hh "com/centreon/broker/bam/kpi_event.hh"
   *  @brief Kpi event
   *
   *  This is the base KPI event that will fill the kpi_events table.
   */
  class          kpi_event : public io::data {
  public:
                 kpi_event();
                 kpi_event(kpi_event const& other);
                 ~kpi_event();
    kpi_event&   operator=(kpi_event const& other);
    unsigned int type() const;

    timestamp    end_time;
    unsigned int kpi_id;
    int          impact_level;
    bool         in_downtime;
    std::string  output;
    std::string  perfdata;
    timestamp    start_time;
    short        status;

  private:
    void         _internal_copy(kpi_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_EVENT_HH
