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
    void    child_has_update(misc::shared_ptr<computable>& child);
    double  impact_hard();
    double  impact_soft();
    bool    in_downtime();
    bool    is_acknowledged();
    void    link_ba(misc::shared_ptr<ba>& my_ba);
    void    unlink_ba();

  private:
    void    _internal_copy(kpi_ba const& right);

    misc::shared_ptr<ba>
            _ba;
    double  _impact_critical;
    double  _impact_warning;
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_BA_HH
