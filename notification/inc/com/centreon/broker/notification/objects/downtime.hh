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

#ifndef CCB_NOTIFICATION_DOWNTIME_HH
#  define CCB_NOTIFICATION_DOWNTIME_HH

#  include <ctime>
#  include <string>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace                 notification {
  namespace               objects {
    /**
     *  @class downtime downtime.hh "com/centreon/broker/notification/objects/downtime.hh"
     *  @brief Downtime object.
     *
     *  The object containing a downtime.
     */
    class                 downtime {
    public:
                          DECLARE_SHARED_PTR(downtime);

                          downtime();
                          downtime(downtime const& obj);
                          downtime& operator=(downtime const& obj);

      time_t              get_entry_time() const throw();
      void                set_entry_time(time_t val) throw();
      std::string const&  get_author() const throw();
      void                set_author(std::string const& val);
      bool                get_cancelled() const throw();
      void                set_cancelled(bool val) throw();
      time_t              get_deletion_time() const throw();
      void                set_deletion_time(time_t val) throw();
      unsigned int        get_duration() const throw();
      void                set_duration(unsigned int val) throw();
      time_t              get_end_time() const throw();
      void                set_end_time(time_t val) throw();
      bool                get_fixed() const throw();
      void                set_fixed(bool val) throw();
      time_t              get_start_time() const throw();
      void                set_start_time(time_t val) throw();
      time_t              get_actual_start_time() const throw();
      void                set_actual_start_time(time_t val) throw();
      time_t              get_actual_end_time() const throw();
      void                set_actual_end_time(time_t val) throw();
      bool                get_started() const throw();
      void                set_started(bool val) throw();
      unsigned int        get_triggered_by() const throw();
      void                set_triggered_by(unsigned int val) throw();
      int                 get_type() const throw();
      void                set_type(int val) throw();

    private:
      time_t              _entry_time;
      unsigned int        _host_id;
      unsigned int        _service_id;
      std::string         _author;
      bool                _cancelled;
      time_t              _deletion_time;
      unsigned int        _duration;
      time_t              _end_time;
      bool                _fixed;
      time_t              _start_time;
      time_t              _actual_start_time;
      time_t              _actual_end_time;
      bool                _started;
      unsigned int        _triggered_by;
      int                 _type;
    };
  }
}

CCB_END()

#endif //!CCB_NOTIFICATION_DOWNTIME_HH
