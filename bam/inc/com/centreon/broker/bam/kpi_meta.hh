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

#ifndef CCB_BAM_KPI_META_HH
#  define CCB_BAM_KPI_META_HH

#  include "com/centreon/broker/bam/kpi.hh"
#  include "com/centreon/broker/bam/kpi_event.hh"
#  include "com/centreon/broker/io/stream.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace     bam {
  // Forward declaration.
  class       meta_service;
  class       computable;

  /**
   *  @class kpi_meta kpi_meta.hh "com/centreon/broker/bam/kpi_meta.hh"
   *  @brief Meta-service as a KPI.
   *
   *  This class allows you to use a meta-service (class meta_service)
   *  as a KPI for a BA.
   */
  class       kpi_meta : public kpi {
  public:
              kpi_meta();
              kpi_meta(kpi_meta const& other);
              ~kpi_meta();
    kpi_meta& operator=(kpi_meta const& other);
    bool      child_has_update(
                computable* child,
                io::stream* visitor = NULL);
    double    get_impact_critical() const;
    double    get_impact_warning() const;
    void      impact_hard(impact_values& hard_impact);
    void      impact_soft(impact_values& soft_impact);
    void      link_meta(misc::shared_ptr<meta_service>& my_meta);
    void      set_impact_critical(double impact);
    void      set_impact_warning(double impact);
    void      unlink_meta();
    void      visit(io::stream* visitor);

  private:
    void      _fill_impact(impact_values& impact);
    void      _internal_copy(kpi_meta const& other);
    void      _open_new_event(
                io::stream* visitor,
                int impact,
                short state);

    misc::shared_ptr<meta_service>
              _meta;
    double    _impact_critical;
    double    _impact_warning;
  };
}

CCB_END()

#endif // !CCB_BAM_KPI_META_HH
