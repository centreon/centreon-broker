/*
** Copyright 2011-2013 Merethis
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

#ifndef CCB_NOTIFICATION_TIMEPERIOD_HH
#  define CCB_NOTIFICATION_TIMEPERIOD_HH

#  include <string>
#  include <vector>
#  include <list>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/group.hh"
#  include "com/centreon/broker/notification/objects/timerange.hh"
#  include "com/centreon/broker/notification/objects/daterange.hh"

CCB_BEGIN()

namespace       notification {

  class         timeperiod {
  public:
    timeperiod();
    timeperiod(timeperiod const& obj);
    timeperiod operator=(timeperiod const& obj);

  private:
    std::string            _alias;
    std::vector<std::list<daterange> >
                           _exceptions;
    group                  _exclude;
    std::string            _timeperiod_name;
    std::vector<std::list<timerange> >
                           _timeranges;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_TIMEPERIOD_HH
