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

#ifndef CCB_BAM_DIMENSION_TIMEPERIOD_EXCLUSION_HH
#  define CCB_BAM_DIMENSION_TIMEPERIOD_EXCLUSION_HH

#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class dimension_timeperiod_exclusion dimension_timeperiod_exclusion.hh "com/centreon/broker/bam/dimension_timeperiod_exclusion.hh"
   *  @brief Timeperiod exclusion.
   *
   *  Declare a timeperiod exclusion.
   */
  class          dimension_timeperiod_exclusion : public io::data {
  public:
                 dimension_timeperiod_exclusion();
                 dimension_timeperiod_exclusion(
                   dimension_timeperiod_exclusion const& other);
                 ~dimension_timeperiod_exclusion();
    dimension_timeperiod_exclusion&
                 operator=(dimension_timeperiod_exclusion const& other);
    unsigned int type() const;
    static unsigned int
                 static_type();

    unsigned int excluded_timeperiod_id;
    unsigned int timeperiod_id;

    static mapping::entry const
                 entries[];
    static io::event_info::event_operations const
                 operations;

  private:
    void         _internal_copy(
                   dimension_timeperiod_exclusion const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_TIMEPERIOD_EXCLUSION_HH
