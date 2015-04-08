/*
** Copyright 2012-2013 Merethis
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

#ifndef CC_DELAYED_DELETE_HH
#  define CC_DELAYED_DELETE_HH

#  include <cstddef>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class delayed_delete delayed_delete.hh "com/centreon/delayed_delete.hh"
 *  @brief Perform a delayed delete.
 *
 *  This template is used to perfom delayed object deletion using a task
 *  manager. The pointer will be deleted when the run() method is
 *  called. If the run() method has not been called when delayed_delete
 *  is destroyed, the pointer won't be deleted.
 *
 *  @see task_manager
 */
template          <typename T>
class             delayed_delete : public task {
public:
  /**
   *  Default constructor.
   *
   *  @param[in] ptr Pointer to delete.
   */
                  delayed_delete(T* ptr) : _ptr(ptr) {}

  /**
   *  Copy constructor.
   *
   *  @param[in] dd Object to copy.
   */
                  delayed_delete(delayed_delete const& dd)
    : task(dd) {
    _internal_copy(dd);
  }

  /**
   *  Destructor.
   */
                  ~delayed_delete() throw () {}

  /**
   *  Assignment operator.
   *
   *  @param[in] ptr Pointer to delete.
   *
   *  @return This object.
   */
  delayed_delete& operator=(delayed_delete const& dd) {
    if (this != &dd) {
      task::operator=(dd);
      _internal_copy(dd);
    }
    return (*this);
  }

  /**
   *  Delete pointer.
   */
  void            run() {
    delete _ptr;
    _ptr = NULL;
    return ;
  }

private:
  /**
   *  Copy internal data members.
   *
   *  @param[in] dd Object to copy.
   */
  void            _internal_copy(delayed_delete const& dd) {
    _ptr = dd._ptr;
    return ;
  }

  T*              _ptr;
};

CC_END()

#endif // !CC_DELAYED_DELETE_HH
