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

#ifndef CC_UNIQUE_ARRAY_PTR_HH
#define CC_UNIQUE_ARRAY_PTR_HH

#include <cstddef>
#include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class unique_array_ptr unique_array_ptr.hh
 *  @brief Similar to unique_ptr for arrays.
 *
 *  Provide similar feature as unique_ptr but for array pointers.
 */
template <typename T>
class unique_array_ptr {
 private:
  T* _ptr;

 public:
  /**
   *  Constructor.
   *
   *  @param[in] t Array pointer.
   */
  unique_array_ptr(T* t = NULL) : _ptr(t) {}

  /**
   *  Copy constructor.
   *
   *  @param[in] uap Object to copy.
   */
  unique_array_ptr(unique_array_ptr& uap) : _ptr(uap._ptr) { uap._ptr = NULL; }

  /**
   *  Destructor.
   */
  ~unique_array_ptr() { delete[] _ptr; }

  /**
   *  Assignment operator.
   *
   *  @param[in] uap Object to copy.
   *
   *  @return This object.
   */
  unique_array_ptr& operator=(unique_array_ptr& uap) {
    if (&uap != this) {
      _ptr = uap._ptr;
      uap._ptr = NULL;
    }
    return (*this);
  }

  /**
   *  Dereferencing pointer.
   *
   *  @return Dereferenced pointer.
   */
  T& operator*() { return (*_ptr); }

  /**
   *  Array access operator.
   *
   *  @param[in] idx Index in array.
   *
   *  @return Element at position idx.
   */
  T& operator[](uint32_t idx) { return (_ptr[idx]); }

  /**
   *  Get the pointer associated with this object.
   *
   *  @return Pointer associated with this object.
   */
  T* get() const { return (_ptr); }

  /**
   *  Release the associated pointer and release it.
   *
   *  @return Pointer associated with this object.
   */
  T* release() {
    T* tmp(_ptr);
    _ptr = NULL;
    return (tmp);
  }

  /**
   *  Reset this automatic pointer.
   *
   *  @param[in] ptr New pointer (can be NULL).
   */
  void reset(T* t = NULL) {
    delete[] _ptr;
    _ptr = t;
    return;
  }
};

CC_END()

#endif  // !CC_UNIQUE_ARRAY_PTR_HH
