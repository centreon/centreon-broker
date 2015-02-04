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

#ifndef CCB_BAM_DIMENSION_TIMEPERIOD_EXCEPTION_HH
#  define CCB_BAM_DIMENSION_TIMEPERIOD_EXCEPTION_HH

#  include <QString>
#  include "com/centreon/broker/io/data.hh"
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace        bam {
  /**
   *  @class dimension_timeperiod_exception dimension_timeperiod_exception.hh "com/centreon/broker/bam/dimension_timeperiod_exception.hh"
   *  @brief Timeperiod exception.
   *
   *  Declare a timeperiod exception.
   */
  class          dimension_timeperiod_exception : public io::data {
  public:
                 dimension_timeperiod_exception();
                 dimension_timeperiod_exception(
                   dimension_timeperiod_exception const& other);
                 ~dimension_timeperiod_exception();
    dimension_timeperiod_exception&
                 operator=(dimension_timeperiod_exception const& other);
    unsigned int type() const;

    QString      daterange;
    QString      timerange;
    unsigned int timeperiod_id;

  private:
    void         _internal_copy(
                   dimension_timeperiod_exception const& other);
  };
}

CCB_END()

#endif // !CCB_BAM_DIMENSION_TIMEPERIOD_EXCEPTION_HH
