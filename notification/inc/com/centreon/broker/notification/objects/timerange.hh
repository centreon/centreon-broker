/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Engine.
**
** Centreon Engine is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Engine is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Engine. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CCB_NOTIFICATION_OBJECTS_TIMERANGE_HH
#  define CCB_NOTIFICATION_OBJECTS_TIMERANGE_HH

#  include <ctime>
#  include <list>
#  include <string>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace         notification {
  namespace       objects {
    /**
     *  @class timerange timerange.hh "com/centreon/broker/notification/objects/timerange.hh"
     *  @brief Timerange object.
     *
     *  The object containing a timerange.
     */
    class           timerange {
    public:
                    timerange(
                      unsigned long start = 0,
                      unsigned long end = 0);
                    timerange(timerange const& right);
                    ~timerange() throw ();
      timerange&    operator=(timerange const& right);
      bool          operator==(timerange const& right) const throw ();
      bool          operator!=(timerange const& right) const throw ();
      bool          operator<(timerange const& right) const throw ();
      unsigned long end() const throw ();
      void          end(unsigned long value);
      unsigned long start() const throw ();
      void          start(unsigned long value);
      unsigned long start_hour() const throw();
      unsigned long start_minute() const throw();
      unsigned long end_hour() const throw();
      unsigned long end_minute() const throw();

      bool          to_time_t(struct tm const& midnight,
                              time_t& range_start,
                              time_t& range_end) const;

      static bool   build_timeranges_from_string(
                      std::string const& line,
                      std::list<timerange>& timeranges);

    private:
      unsigned long _end;
      unsigned long _start;
    };
  }
}

CCB_END()

#endif // !CCB_NOTIFICATION_OBJECTS_TIMERANGE_HH
