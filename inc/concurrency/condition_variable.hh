/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#ifndef CONCURRENCY_CONDITION_VARIABLE_HH_
# define CONCURRENCY_CONDITION_VARIABLE_HH_

# include <sys/types.h>

namespace               concurrency {
  // Forward declaration.
  class                 mutex;

  /**
   *  @class condition_variable condition_variable.hh "concurrency/condition_variable.hh"
   *  @brief Condition variable.
   *
   *  Represents a condition variable.
   */
  class                 condition_variable {
   private:
    pthread_cond_t      _cv;
                        condition_variable(condition_variable const& cv);
    condition_variable& operator=(condition_variable const& cv);

   public:
                        condition_variable();
                        ~condition_variable();
    void                sleep(mutex& m);
    bool                sleep(mutex& m, time_t deadline);
    void                wake();
    void                wake_all();
  };
}

#endif /* !CONCURRENCY_CONDITION_VARIABLE_HH_ */
