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

    std::string const&     get_alias() const throw();
    void                   set_alias(std::string const& value);

    std::vector<std::list<daterange> > const&
                           get_exceptions() const throw();
    void                   add_exceptions(std::list<daterange> const& val);

    group const&           get_exclude() const throw();
    void                   set_exclude(group const& val);

    std::string const&     get_timeperiod_name() const throw();
    void                   set_timeperiod_name(std::string const& value);

    std::vector<std::list<timerange> > const&
                           get_timeranges() const throw();
    void                   add_timerange(std::list<timerange> const& val);
    void                   set_timerange(std::list<timerange> const& val,
                                         int day);
    std::list<timerange> const&
                           get_timeranges_by_day(int day) const throw();

    std::string const&     get_timezone() const throw();
    void                   set_timezone(std::string const& tz);

    bool                   is_valid(time_t preferred_time) const;
    time_t                 get_next_valid(time_t preferred_time) const;

  private:
    std::string            _alias;
    std::vector<std::list<daterange> >
                           _exceptions;
    group                  _exclude;
    std::string            _timeperiod_name;
    std::vector<std::list<timerange> >
                           _timeranges;
    std::string            _timezone;
  };

}

CCB_END()

#endif // !CCB_NOTIFICATION_TIMEPERIOD_HH
