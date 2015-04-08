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

#ifndef CC_HANDLE_ACTION_HH
#  define CC_HANDLE_ACTION_HH

#  include "com/centreon/namespace.hh"
#  include "com/centreon/task.hh"

CC_BEGIN()

// Forward declaration.
class              handle;
class              handle_listener;

/**
 *  @class handle_action handle_action.hh "com/centreon/handle_action.hh"
 *  @brief Notify a listener.
 *
 *  Notify a listener from a handle event.
 */
class              handle_action : public task {
public:
  enum             action {
    none = 0,
    read,
    write,
    error
  };

                   handle_action(
                     handle* h,
                     handle_listener* hl,
                     bool is_threadable = false);
                   handle_action(handle_action const& right);
                   ~handle_action() throw ();
  handle_action&   operator=(handle_action const& right);
  bool             is_threadable() const throw ();
  handle*          get_handle() const throw ();
  handle_listener* get_handle_listener() const throw ();
  void             run();
  void             set_action(action a) throw ();

private:
  void             _internal_copy(handle_action const& right);

  action           _action;
  handle*          _h;
  handle_listener* _hl;
  bool             _is_threadable;
};

CC_END()

#endif // !CC_HANDLE_ACTION_HH
