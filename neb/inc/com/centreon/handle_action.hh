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

#ifndef CC_HANDLE_ACTION_HH
#define CC_HANDLE_ACTION_HH

#include <atomic>
#include "com/centreon/namespace.hh"
#include "com/centreon/task.hh"

CC_BEGIN()

// Forward declaration.
class handle;
class handle_listener;

/**
 *  @class handle_action handle_action.hh "com/centreon/handle_action.hh"
 *  @brief Notify a listener.
 *
 *  Notify a listener from a handle event.
 */
class handle_action : public task {
 public:
  enum action { none = 0, read, write, error };

  handle_action(handle* h, handle_listener* hl, bool is_threadable = false);
  handle_action(handle_action const& right) = delete;
  ~handle_action() noexcept;
  handle_action& operator=(handle_action const& right) = delete;
  bool is_threadable() const noexcept;
  handle* get_handle() const noexcept;
  handle_listener* get_handle_listener() const noexcept;
  void run();
  void set_action(action a) noexcept;

 private:
  void _internal_copy(handle_action const& right);

  std::atomic<action> _action;
  handle* _h;
  handle_listener* _hl;
  bool _is_threadable;
};

CC_END()

#endif  // !CC_HANDLE_ACTION_HH
