/*
** Copyright 2012-2013,2019-2021 Centreon
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
 *  This class is a singleton, it manages processes by doing two things:
 *  * waitpid() so it knows when a process is over.
 *  * poll() so it knows when operations are available on fds.
 *
 *  This singleton starts a thread running the main loop inside the _run()
 *  method. This method is executed with a condition variable _running_cv and a
 *  boolean _running. The constructor is released only when the boolean is set
 *  to true, that is to say, the loop is really started.
 *
 *  Once the loop is correctly started, the user can add to it processes. This
 *  is done with the add() method. An important point is poll() and waitpid()
 *  are called one after the other. And it is really better if no processes
 *  are added between the two calls. So to avoid this case, processes are
 *  inserted in the manager during the _update_list() internal function.
 *
 *  A good point that comes with this fact, is that we don't need mutex to
 *  access data in the manager.
 *
 *  The add() method locks a mutex _add_m and fills a queue _processes then
 *  set the _update flag to true. Since a process can be closed very quickly
 *  the _processes queue contains a pair with the pid and the process, because
 *  when a process finishes, its _process attribute (the pid) is set to -1, so
 *  we could loose its original value.
 *
 *  When _update_list() is called, _add_m is locked, the queue is exchanged with
 *  an internal one and _update is set to false. Then _update_list() can work
 *  on its queue without being disturbed by new coming processes.
 *
 *  The main goal of _update_list() is to update various tables and arrays, the
 *  main ones are:
 *  * _fds which is an array of struct pollfd needed by the poll() function.
 *  * _processes_fd which is a table keeping relations between fds and
 *    processes.
 *  * _processes_pid which is a table giving relations between pids and
 *    processes.
 *  * _processes_timeout which gives the time limit of a process, after this
 *    time, the process is killed.
 *  * We also have _orphans_pid that is almost empty. But it is not always the
 *    case. Processes can be launched before they are referenced into _fds and
 *    the several tables. In that case, particularly when they finish quickly
 *    they may be catch by the waitpid function. And since we don't have them
 *    in _processes_pid and others, we store them in _orphans_pid. Then later,
 *    they should appear in others tables and the manager will be able to clear
 *    them correctly.
 *
 *  The class attributes:
 *  * _running is a boolean telling if the main loop is running.
 *  * _running_m/_running_cv are mutex/condition_variable controlling the start
 *    of the main loop and also used to tell a process, it is no more referenced
 *    in the manager (see _wait_for_update() method).
 *  * _finished is a boolean telling when the destructor has been called. All
 *    the stuff needed to stop the loop are done in it, this is because the
 *    destructor may be called from another thread and we want to avoid mutexes.
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
  std::unordered_map<int32_t, process*> _processes_fd;
  std::atomic_bool _running;
  std::atomic_bool _finished;
  mutable std::mutex _running_m;
  mutable std::condition_variable _running_cv;
  std::thread _thread;

  std::deque<orphan> _orphans_pid;
  std::unordered_map<pid_t, process*> _processes_pid;
  mutable std::mutex _timeout_m;
  std::multimap<uint32_t, process*> _processes_timeout;

  mutable std::mutex _add_m;
  std::deque<std::pair<pid_t, process*>> _processes;

  process_manager();
  ~process_manager() noexcept;
  void _close_stream(int fd) noexcept;
  void _erase_timeout(process* p);
  void _kill_processes_timeout() noexcept;
  uint32_t _read_stream(int fd) noexcept;
  void _run();
  void _update_ending_process(process* p, int status) noexcept;
  void _update_list();
  void _wait_orphans_pid() noexcept;
  void _wait_processes() noexcept;
  void _stop_processes() noexcept;

 public:
  void add(process* p);
  static process_manager& instance();
  process_manager& operator=(process_manager const& p) = delete;
  process_manager(process_manager const& p) = delete;
  void wait_for_update() const noexcept;
};

CC_END()

#endif  // !CC_PROCESS_MANAGER_POSIX_HH
