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

#ifndef CCB_BAM_KPI_HH
#  define CCB_BAM_KPI_HH

#  include "com/centreon/broker/bam/computable.hh"
#  include "com/centreon/broker/bam/kpi.hh"

namespace          bam {
  /**
   *  @class kpi kpi.hh "com/centreon/broker/bam/kpi.hh"
   *  @brief Impact of a BA.
   *
   *  This is the base class that represents an impact of a BA. This
   *  can either be a boolean rule or a service or a BA itself.
   */
  class            kpi : public computable {
  public:
                   kpi();
                   kpi(kpi const& right);
    virtual        ~kpi();
    kpi&           operator=(kpi const& right);
    virtual double impact_hard() = 0;
    virtual double impact_soft() = 0;
    virtual bool   in_downtime() = 0;
    virtual bool   is_acknowledged() = 0;
  };
}

#endif // !CCB_BAM_KPI_HH
