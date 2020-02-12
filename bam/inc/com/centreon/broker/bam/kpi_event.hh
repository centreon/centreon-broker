/*
** Copyright 2014 Centreon
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

#ifndef CCB_BAM_KPI_EVENT_HH
#  define CCB_BAM_KPI_EVENT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

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
    bool         operator==(kpi_event const& other) const;
    unsigned int type() const;
    static unsigned int
                 static_type();

    timestamp    end_time;
    unsigned int kpi_id;
    int          impact_level;
    bool         in_downtime;
    QString      output;
    QString      perfdata;
    timestamp    start_time;
    short        status;
    unsigned int ba_id;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void         _internal_copy(kpi_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_EVENT_HH
