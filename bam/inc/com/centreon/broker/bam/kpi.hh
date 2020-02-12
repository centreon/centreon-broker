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

#ifndef CCB_BAM_KPI_HH
#  define CCB_BAM_KPI_HH

#  include <vector>
#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/bam/kpi_event.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  // Forward declarations.
  class          ba;
  class          impact_values;

  /**
   *  @class kpi kpi.hh "com/centreon/broker/bam/kpi.hh"
   *  @brief Impact of a BA.
   *
   *  This is the base class that represents an impact of a BA. This
   *  can either be a boolean rule or a service or a BA itself.
   */
  class          kpi : public computable {
  public:
                 kpi();
                 kpi(kpi const& right);
    virtual      ~kpi();
    kpi&         operator=(kpi const& right);
    unsigned int get_id() const;
    unsigned int get_ba_id() const;
    timestamp    get_last_state_change() const;
    virtual void impact_hard(impact_values& hard_impact) = 0;
    virtual void impact_soft(impact_values& soft_impact) = 0;
    void         set_id(unsigned int id);
    void         set_ba_id(unsigned int id);
    virtual void set_initial_event(kpi_event const& e);
    virtual void visit(io::stream* visitor) = 0;
    virtual bool in_downtime() const;
    virtual bool ok_state() const = 0;

    void         commit_initial_events(io::stream* visitor);

  protected:
    unsigned int _id;
    unsigned int _ba_id;
    misc::shared_ptr<kpi_event>
                 _event;
    std::vector<misc::shared_ptr<kpi_event> >
                 _initial_events;
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_HH
