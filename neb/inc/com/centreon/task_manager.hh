/*
** Copyright 2011-2019 Centreon
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

#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include <vector>
#include "com/centreon/namespace.hh"
#include "com/centreon/task.hh"
#include "com/centreon/timestamp.hh"

#include <iostream>

CC_BEGIN()

class task_manager {
  struct internal_task {
    uint64_t id;
    bool is_runnable;
    bool should_delete;
    uint32_t interval;  // When 0, this task is in auto_delete
    task* tsk;

    internal_task(task* tsk,
                  uint64_t id,
                  uint32_t interval,
                  bool is_runnable,
                  bool should_delete)
        : id{id},
          is_runnable{is_runnable},
          should_delete{should_delete},
          interval{interval},
          tsk{tsk} {}
    internal_task() = delete;
  };

  uint64_t _current_id;
  bool _exit;

  std::vector<std::thread> _workers;

  mutable std::mutex _tasks_m;
  std::multimap<timestamp, internal_task*> _tasks;

  mutable std::mutex _queue_m;
  mutable std::condition_variable _queue_cv;
  std::deque<internal_task*> _queue;

  void _enqueue(internal_task* t);
  void _wait_for_queue_empty() const;

 public:
  task_manager(uint32_t max_thread_count = 0);
  ~task_manager();

  uint64_t add(task* t,
               timestamp const& when,
               bool is_runnable = false,
               bool should_delete = false);

  uint64_t add(task* t,
               timestamp const& when,
               uint32_t interval,
               bool is_runnable = false,
               bool should_delete = false);
  timestamp next_execution_time() const;
  uint32_t remove(task* t);
  bool remove(uint64_t id);
  uint32_t execute(timestamp const& now = timestamp::now());
};

CC_END()

#endif  // ! CC_TASK_MANAGER_HH
