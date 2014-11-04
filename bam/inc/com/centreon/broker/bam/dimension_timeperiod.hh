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

#  include <memory>
#  include <vector>
#  include <QSqlDatabase>
#  include <QSqlQuery>
#  include <QThread>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/bam/time/timeperiod.hh"

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

    time::timeperiod::ptr
                  timeperiod;
  private:
    void           _internal_copy(dimension_timeperiod const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_TIMEPERIOD_HH
