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

#ifndef CCB_BAM_DIMENSION_BA_EVENT_HH
#  define CCB_BAM_DIMENSION_BA_EVENT_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/timestamp.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace                bam {
  /**
   *  @class dimension_ba_event dimension_ba_event.hh "com/centreon/broker/bam/dimension_ba_event.hh"
   *  @brief Dimension Ba event
   *
   */
  class                  dimension_ba_event : public io::data {
  public:
                         dimension_ba_event();
                         dimension_ba_event(
                           dimension_ba_event const& other);
                         ~dimension_ba_event();
    dimension_ba_event&  operator=(dimension_ba_event const& other);
    bool                 operator==(
                           dimension_ba_event const& other) const;
    unsigned int         type() const;
    static unsigned int  static_type();

    unsigned int         ba_id;
    QString              ba_name;
    QString              ba_description;
    double               sla_month_percent_crit;
    double               sla_month_percent_warn;
    unsigned int         sla_duration_crit;
    unsigned int         sla_duration_warn;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void                 _internal_copy(
                           dimension_ba_event const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_BA_EVENT_HH
