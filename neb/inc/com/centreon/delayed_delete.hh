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

#ifndef CC_DELAYED_DELETE_HH
#define CC_DELAYED_DELETE_HH

#include <cstddef>
#include "com/centreon/namespace.hh"

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
template <typename T>
class delayed_delete : public task {
  T* _ptr;
  /**
   *  Copy internal data members.
   *
   *  @param[in] dd Object to copy.
   */
  void _internal_copy(delayed_delete const& dd) {
    _ptr = dd._ptr;
  }

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
  delayed_delete(delayed_delete const& dd) : task(dd) { _internal_copy(dd); }

  /**
   *  Destructor.
   */
  ~delayed_delete() noexcept {}

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
    return *this;
  }

  /**
   *  Delete pointer.
   */
  void run() {
    delete _ptr;
    _ptr = NULL;
  }
};

CC_END()

#endif  // !CC_DELAYED_DELETE_HH
