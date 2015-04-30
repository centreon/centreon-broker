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

#ifndef CCB_BAM_DIMENSION_TIMEPERIOD_HH
#  define CCB_BAM_DIMENSION_TIMEPERIOD_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/io/event_info.hh"
#  include "com/centreon/broker/mapping/entry.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class dimension_timeperiod dimension_timeperiod.hh "com/centreon/broker/bam/dimension_timeperiod.hh"
   *  @brief Timeperiod dimension
   *
   */
  class            dimension_timeperiod : public io::data {
  public:
                   dimension_timeperiod();
                   ~dimension_timeperiod();
                   dimension_timeperiod(dimension_timeperiod const&);
    dimension_timeperiod&
                   operator=(dimension_timeperiod const&);
    bool           operator==(dimension_timeperiod const& other) const;
    unsigned int   type() const;
    static unsigned int
                   static_type();

    unsigned int   id;
    QString        name;
    QString        monday;
    QString        tuesday;
    QString        wednesday;
    QString        thursday;
    QString        friday;
    QString        saturday;
    QString        sunday;

    static mapping::entry const
                   entries[];
    static io::event_info::event_operations const
                   operations;

  private:
    void           _internal_copy(dimension_timeperiod const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_TIMEPERIOD_HH
