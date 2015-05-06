/*
** Copyright 2011-2014 Merethis
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

#ifndef CCB_NEB_TIMEPERIOD_LINKER_HH
#  define CCB_NEB_TIMEPERIOD_LINKER_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/neb/node_id.hh"
#  include "com/centreon/broker/time/timeperiod.hh"
#  include "com/centreon/broker/neb/timeperiod_builder.hh"

CCB_BEGIN()

namespace       neb {
  /**
   *  @class timeperiod_linker timeperiod_linker.hh "com/centreon/broker/neb/timeperiod_linker.hh"
   *  @brief Timeperiod linker.
   *
   *  Links the timeperiod together.
   */
  class         timeperiod_linker
                  : public timeperiod_builder {
  public:
    timeperiod_linker();

    void        add_timeperiod(
                  unsigned int id,
                  time::timeperiod::ptr con);

    void        add_timeperiod_exception(
                  unsigned int timeperiod_id,
                  std::string const& days,
                  std::string const& timerange);
    void        add_timeperiod_exclude_relation(
                  unsigned int timeperiod_id,
                  unsigned int exclude_id);
    void        add_timeperiod_include_relation(
                  unsigned int timeperiod_id,
                  unsigned int include_id);

  private:
    QHash<unsigned int, time::timeperiod::ptr>
                  _table;
  };

}

CCB_END()

#endif // !CCB_NEB_TIMEPERIOD_LINKER_HH
