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

#ifndef CC_HANDLE_MANAGER_POSIX_HH
#define CC_HANDLE_MANAGER_POSIX_HH

#include <poll.h>
#include <map>
#include <utility>
#include "com/centreon/handle.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

// Forward declarations.
class handle_action;
class handle_listener;
class task_manager;

/**
 *  @class handle_manager handle_manager_posix.hh
 * "com/centreon/handle_manager.hh"
 *  @brief Multiplex I/O from multiple handles.
 *
 *  Listen handles and notifies listeners accordingly.
 */
class handle_manager {
 public:
  handle_manager(task_manager* tm = NULL);
  handle_manager(handle_manager const& right);
  virtual ~handle_manager() throw();
  handle_manager& operator=(handle_manager const& right);
  void add(handle* h, handle_listener* hl, bool is_threadable = false);
  void link(task_manager* tm);
  void multiplex();
  bool remove(handle* h);
  uint32_t remove(handle_listener* hl);

 private:
  void _internal_copy(handle_manager const& right);
  static int _poll(pollfd* fds, nfds_t nfds, int timeout) throw();
  void _setup_array();

  pollfd* _array;
  std::map<native_handle, handle_action*> _handles;
  bool _recreate_array;
  task_manager* _task_manager;
};

CC_END()

#endif  // !CC_HANDLE_MANAGER_POSIX_HH
