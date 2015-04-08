/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it
** and/or modify it under the terms of the GNU Affero General Public
** License as published by the Free Software Foundation, either version
** 3 of the License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be
** useful, but WITHOUT ANY WARRANTY; without even the implied warranty
** of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_TASK_HH
#  define CC_TASK_HH

#  include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class task task.hh "com/centreon/task.hh"
 *  @brief Base for all task objects.
 *
 *  This class is an interface for piece of code to needs to be
 *  manage by the task manager.
 */
class          task {
public:
               task();
               task(task const& t);
  virtual      ~task() throw ();
  task&        operator=(task const& t);
  virtual void run();
};

CC_END()

#endif // !CC_TASK_HH
