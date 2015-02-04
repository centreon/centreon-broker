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

#ifndef CCB_BAM_KPI_STATUS_HH
#  define CCB_BAM_KPI_STATUS_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class kpi_status kpi_status.hh "com/centreon/broker/bam/kpi_status.hh"
   *  @brief Update status of a KPI.
   *
   *  Update the status of a KPI, used to update the mod_bam_kpi table.
   */
  class          kpi_status : public io::data {
  public:
                 kpi_status();
                 kpi_status(kpi_status const& other);
                 ~kpi_status();
    kpi_status&  operator=(kpi_status const& other);
    unsigned int type() const;

    unsigned int kpi_id;
    double       level_acknowledgement_hard;
    double       level_acknowledgement_soft;
    double       level_downtime_hard;
    double       level_downtime_soft;
    double       level_nominal_hard;
    double       level_nominal_soft;
    short        state_hard;
    short        state_soft;
    timestamp    last_state_change;
    double       last_impact;

  private:
    void         _internal_copy(kpi_status const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_STATUS_HH
