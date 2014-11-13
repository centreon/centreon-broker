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

#ifndef CCB_BAM_TIMEPERIOD_MAP_HH
#  define CCB_BAM_TIMEPERIOD_MAP_HH

#  include <map>
#  include <memory>
#  include <QMutex>
#  include <QMutexLocker>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/bam/time/timeperiod.hh"

CCB_BEGIN()

namespace          bam {
  /**
   *  @class timeperiod_map timeperiod_map.hh "com/centreon/broker/bam/timeperiod_map.hh"
   *  @brief Timeperiod map made to be shared between threads.
   */
  class            timeperiod_map {
  public:
                   timeperiod_map();
                   ~timeperiod_map();
                   timeperiod_map(timeperiod_map const&);
    timeperiod_map&
                   operator=(timeperiod_map const&);
    bool           operator==(timeperiod_map const& other) const;

    std::auto_ptr<QMutexLocker>
                   lock();
    time::timeperiod::ptr
                   get_timeperiod(unsigned int id);
    void           add_timeperiod(unsigned int id,
                                  time::timeperiod::ptr ptr);
    void           clear();

  private:
    QMutex        _mutex;
    std::map<unsigned int, time::timeperiod::ptr>
                  _map;
  };
}

CCB_END()

#endif // !CCB_BAM_TIMEPERIOD_MAP_HH
