/*
** Copyright 2012-2013,2019 Centreon
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

#ifndef CC_PROCESS_MANAGER_POSIX_HH
#define CC_PROCESS_MANAGER_POSIX_HH

#include <poll.h>
#include <atomic>
#include <condition_variable>
#include <deque>
#include <map>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include "com/centreon/namespace.hh"

CC_BEGIN()

class process;
class process_listener;

/**
 *  @class process_manager process_manager_posix.hh
 *"com/centreon/process_manager_posix.hh"
 *  @brief This class manage process.
 *
 *  This class is a singleton, it manages processes. When it is instantiated, it
 *starts a thread running the main loop inside the _run() method. This _run
 *method is executed with a condition variable _running_cv and a boolean
 *running. The constructor is released only when the boolean is set to true,
 *that is to say, the loop is really started.
 *
 *  Once the loop is correctly started, the user can add to it processes. This
 *is done with the add() method. Since the main loop is running while we add a
 *process, a mutex _lock_processes is used. During the add action,
 *  * A map _processes_fd is completed, this one returns a process knowing its
 *output fd or its error fd.
 *  * If the process is configured with a timeout, the table _processes_timeout
 *is also filled ; with it, from a timeout, we can get its process. It is a
 *multimap.
 *  * It is also asked the file descriptors list to be updated by setting the
 *_update boolean to true.
 *  * The process is stored in the _processes_pid table, here it is stored by
 *pid.
 */
class process_manager {
  struct orphan {
    orphan(pid_t _pid = 0, int _status = 0) : pid(_pid), status(_status) {}
    pid_t pid;
    int status;
  };
  /**
   * A boolean set to true when file descriptors list needs to be updated.
   */
  std::atomic_bool _update;
  std::vector<pollfd> _fds;
  /**
   * Here is a boolean telling if the main loop is running or not. This variable
   * is set to true when the main loop starts and is set to false by the
   * process_manager destructor when we want to stop it.
   */
  std::atomic_bool _running;
  std::mutex _running_m;
  std::condition_variable _running_cv;
  std::thread _thread;

  mutable std::mutex _lock_processes;
  std::unordered_map<int32_t, process*> _processes_fd;
  std::deque<orphan> _orphans_pid;
  std::unordered_map<pid_t, process*> _processes_pid;
  std::multimap<uint32_t, process*> _processes_timeout;

  process_manager();
  ~process_manager() noexcept;
  static void _close(int& fd) noexcept;
  void _close_stream(int fd) noexcept;
  void _erase_timeout(process* p);
  void _kill_processes_timeout() noexcept;
  uint32_t _read_stream(int fd) noexcept;
  void _run();
  void _update_ending_process(process* p, int status) noexcept;
  void _update_list();
  void _wait_orphans_pid() noexcept;
  void _wait_processes() noexcept;

 public:
  void add(process* p);
  static process_manager& instance();
  process_manager& operator=(process_manager const& p) = delete;
  process_manager(process_manager const& p) = delete;
};

CC_END()

#endif  // !CC_PROCESS_MANAGER_POSIX_HH
