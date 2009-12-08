/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef CONCURRENCY_CONDITION_VARIABLE_H_
# define CONCURRENCY_CONDITION_VARIABLE_H_

# include <sys/types.h> // for pthread_cond_t

namespace              Concurrency
{
  // Forward declaration.
  class                Mutex;

  /**
   *  \class ConditionVariable condition_variable.h "concurrency/condition_variable.h"
   *  \brief Condition variable.
   *
   *  Represents a condition variable.
   */
  class                ConditionVariable
  {
   private:
    pthread_cond_t     cv_;
                       ConditionVariable(const ConditionVariable& cv);
    ConditionVariable& operator=(const ConditionVariable& cv);

   public:
                       ConditionVariable();
                       ~ConditionVariable();
    void               Sleep(Mutex& mutex);
    void               Wake();
    void               WakeAll();
  };
}

#endif /* !CONCURRENCY_CONDITION_VARIABLE_H_ */
