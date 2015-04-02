/*
** Copyright 2014 Merethis
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

#ifndef CCB_CORE_TIME_TIMEZONE_LOCKER_HH
#  define CCB_CORE_TIME_TIMEZONE_LOCKER_HH

#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace time {
/**
 *  @class timezone_locker timezone_locker.hh "com/centreon/broker/time/timezone_locker.hh"
 *  @brief Handle timezone changes, even in case of exception.
 *
 *  This class works on a timezone_manager to push a new timezone at
 *  construction and pop it when destructed.
 */
  class                 timezone_locker {
  public:
                        timezone_locker(char const* tz);
                        ~timezone_locker();

  private:
                        timezone_locker(timezone_locker const& other);
    timezone_locker&    operator=(timezone_locker const& other);
  };
}

CCB_END()

#endif // !CCB_CORE_TIME_TIMEZONE_LOCKER_HH
