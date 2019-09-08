/*
** Copyright 2011-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#ifndef CC_TASK_HH
#define CC_TASK_HH

#include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class task task.hh "com/centreon/task.hh"
 *  @brief Base for all task objects.
 *
 *  This class is an interface for piece of code to needs to be
 *  manage by the task manager.
 */
class task {
 public:
  task();
  task(task const& t);
  virtual ~task() throw();
  task& operator=(task const& t);
  virtual void run();
};

CC_END()

#endif  // !CC_TASK_HH
