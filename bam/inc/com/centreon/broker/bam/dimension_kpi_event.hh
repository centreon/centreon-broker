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

#ifndef CCB_BAM_DIMENSION_KPI_EVENT_HH
#  define CCB_BAM_DIMENSION_KPI_EVENT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class dimension_kpi_event dimension_kpi_event.hh "com/centreon/broker/bam/dimension_kpi_event.hh"
   *  @brief Dimension Kpi event
   *
   */
  class                  dimension_kpi_event : public io::data {
  public:
                         dimension_kpi_event();
                         dimension_kpi_event(dimension_kpi_event const& other);
                         ~dimension_kpi_event();
    dimension_kpi_event& operator=(dimension_kpi_event const& other);
    bool                 operator==(dimension_kpi_event const& other) const;
    unsigned int         type() const;

    unsigned             kpi_id;
    unsigned int         ba_id;
    QString              ba_name;
    unsigned int         host_id;
    QString              host_name;
    unsigned int         service_id;
    QString              service_description;
    unsigned int         kpi_ba_id;
    QString              kpi_ba_name;
    unsigned int         meta_service_id;
    QString              meta_service_name;
    unsigned int         boolean_id;
    QString              boolean_name;
    double               impact_warning;
    double               impact_critical;
    double               impact_unknown;

  private:
    void         _internal_copy(dimension_kpi_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_KPI_EVENT_HH
