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

#ifndef CC_TASK_MANAGER_HH
#define CC_TASK_MANAGER_HH

#include <ctime>
#include <map>
#include "com/centreon/concurrency/mutex.hh"
#include "com/centreon/concurrency/thread_pool.hh"
#include "com/centreon/namespace.hh"
#include "com/centreon/task.hh"
#include "com/centreon/timestamp.hh"

CC_BEGIN()

/**
 *  @class task_manager task_manager.hh "com/centreon/task_manager.hh"
 *  @brief Provide task manager.
 *
 *  Allow to manage task, run tasks on time and run them if possible
 *  in multiple threads.
 */
class task_manager {
 public:
  task_manager(unsigned int max_thread_count = 0);
  virtual ~task_manager() throw();
  unsigned long add(task* t,
                    timestamp const& when,
                    bool is_runnable = false,
                    bool should_delete = false);
  unsigned long add(task* t,
                    timestamp const& when,
                    unsigned int interval,
                    bool is_runnable = false,
                    bool should_delete = false);
  unsigned int execute(timestamp const& now = timestamp::now());
  timestamp next_execution_time() const;
  unsigned int remove(task* t);
  bool remove(unsigned long id);

 private:
  struct internal_task : public concurrency::runnable {
    internal_task(unsigned long id = 0,
                  task* t = NULL,
                  timestamp const& when = 0,
                  unsigned int interval = 0,
                  bool is_runnable = false,
                  bool should_delete = false);
    internal_task(internal_task const& right);
    ~internal_task() throw();
    internal_task& operator=(internal_task const& right);
    void run();

    unsigned long id;
    unsigned int interval;
    bool is_runnable;
    bool should_delete;
    task* t;
    timestamp when;

   private:
    internal_task& _internal_copy(internal_task const& right);
  };

  task_manager(task_manager const& right);
  task_manager& operator=(task_manager const& right);

  unsigned long _current_id;
  mutable concurrency::mutex _mtx;
  std::multimap<timestamp, internal_task*> _tasks;
  concurrency::thread_pool _th_pool;
};

CC_END()

#endif  // !CC_TASK_MANAGER_HH
