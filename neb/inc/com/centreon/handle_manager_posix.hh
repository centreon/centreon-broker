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

#ifndef CC_HANDLE_MANAGER_POSIX_HH
#  define CC_HANDLE_MANAGER_POSIX_HH

#  include <map>
#  include <poll.h>
#  include <utility>
#  include "com/centreon/namespace.hh"
#  include "com/centreon/handle.hh"

CC_BEGIN()

// Forward declarations.
class             handle_action;
class             handle_listener;
class             task_manager;

/**
 *  @class handle_manager handle_manager_posix.hh "com/centreon/handle_manager.hh"
 *  @brief Multiplex I/O from multiple handles.
 *
 *  Listen handles and notifies listeners accordingly.
 */
class             handle_manager {
public:
                  handle_manager(task_manager* tm = NULL);
                  handle_manager(handle_manager const& right);
  virtual         ~handle_manager() throw ();
  handle_manager& operator=(handle_manager const& right);
  void            add(
                    handle* h,
                    handle_listener* hl,
                    bool is_threadable = false);
  void            link(task_manager* tm);
  void            multiplex();
  bool            remove(handle* h);
  unsigned int    remove(handle_listener* hl);

private:
  void            _internal_copy(handle_manager const& right);
  static int      _poll(
                    pollfd *fds,
                    nfds_t nfds,
                    int timeout) throw ();
  void            _setup_array();

  pollfd*         _array;
  std::map<native_handle, handle_action*>
                  _handles;
  bool            _recreate_array;
  task_manager*   _task_manager;
};

CC_END()

#endif // !CC_HANDLE_MANAGER_POSIX_HH
