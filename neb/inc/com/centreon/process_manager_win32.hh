/*
** Copyright 2012-2013 Centreon
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

#ifndef CC_PROCESS_MANAGER_WIN32_HH
#define CC_PROCESS_MANAGER_WIN32_HH

#include <poll.h>
#include "com/centreon/concurrency/mutex.hh"
#include "com/centreon/concurrency/thread.hh"
#include "com/centreon/htable.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

class process;
class process_listener;

/**
 *  @class process_manager process_manager_win32.hh
 * "com/centreon/process_manager_win32.hh"
 *  @brief This class manage process.
 *
 *  This class is a singleton and manage process.
 */
class process_manager : public concurrency::thread {
 public:
  void add(process* p);
  static process_manager& instance();
  static void load();
  static void unload();

 private:
  process_manager();
  process_manager(process_manager const& p);
  ~process_manager() throw();
  process_manager& operator=(process_manager const& p);
  void _close_stream(HANDLE fd) throw();
  void _read_stream(HANDLE fd) throw();
  void _run();
  void _update_list();
  void _wait_processes() throw();

  HANDLE* _fds;
  uint32_t _fds_capacity;
  uint32_t _fds_size;
  concurrency::mutex _lock_processes;
  htable<HANDLE, process*> _processes_fd;
  // htable<pid_t, process*>  _processes_pid;
  bool _quit;
  bool _update;
};

CC_END()

#endif  // !CC_PROCESS_MANAGER_WIN32_HH
