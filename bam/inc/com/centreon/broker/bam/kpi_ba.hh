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

#ifndef CCB_BAM_KPI_BA_HH
#  define CCB_BAM_KPI_BA_HH

#  include "com/centreon/broker/bam/kpi.hh"
#  include "com/centreon/broker/bam/kpi_event.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace   bam {
  // Forward declaration.
  class     ba;
  class     computable;

  /**
   *  @class kpi_ba kpi_ba.hh "com/centreon/broker/bam/kpi_ba.hh"
   *  @brief BA as a KPI.
   *
   *  This class allows you to use a BA (class ba) as a KPI for another
   *  BA.
   */
  class     kpi_ba : public kpi {
  public:
            kpi_ba();
            kpi_ba(kpi_ba const& right);
            ~kpi_ba();
    kpi_ba& operator=(kpi_ba const& right);
    bool    child_has_update(
              computable* child,
              monitoring_stream* visitor = NULL);
    double  get_impact_critical() const;
    double  get_impact_warning() const;
    void    impact_hard(impact_values& hard_impact);
    void    impact_soft(impact_values& soft_impact);
    void    link_ba(misc::shared_ptr<ba>& my_ba);
    void    set_impact_critical(double impact);
    void    set_impact_warning(double impact);
    void    unlink_ba();
    void    visit(monitoring_stream* visitor);

  private:
    void    _fill_impact(
              impact_values& impact,
              short state,
              double acknowledgement,
              double downtime);
    void    _internal_copy(kpi_ba const& right);
    void    _open_new_event(monitoring_stream* visitor, int impact);

    misc::shared_ptr<ba>
            _ba;
    double  _impact_critical;
    double  _impact_warning;
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_BA_HH
