/*
** Copyright 2011-2013 Merethis
**
** This file is part of Centreon Clib.
**
** Centreon Clib is free software: you can redistribute it and/or modify
** it under the terms of the GNU Affero General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
**
** Centreon Clib is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** Affero General Public License for more details.
**
** You should have received a copy of the GNU Affero General Public
** License along with Centreon Clib. If not, see
** <http://www.gnu.org/licenses/>.
*/

#ifndef CC_UNIQUE_ARRAY_PTR_HH
#  define CC_UNIQUE_ARRAY_PTR_HH

#  include <cstddef>
#  include "com/centreon/namespace.hh"

CC_BEGIN()

/**
 *  @class unique_array_ptr unique_array_ptr.hh
 *  @brief Similar to unique_ptr for arrays.
 *
 *  Provide similar feature as unique_ptr but for array pointers.
 */
template     <typename T>
class        unique_array_ptr {
 private:
  T*         _ptr;

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
             unique_array_ptr(unique_array_ptr& uap) : _ptr(uap._ptr) {
    uap._ptr = NULL;
  }

  /**
   *  Destructor.
   */
             ~unique_array_ptr() {
    delete [] _ptr;
  }

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
  T&         operator*() {
    return (*_ptr);
  }

  /**
   *  Array access operator.
   *
   *  @param[in] idx Index in array.
   *
   *  @return Element at position idx.
   */
  T&         operator[](unsigned int idx) {
    return (_ptr[idx]);
  }

  /**
   *  Get the pointer associated with this object.
   *
   *  @return Pointer associated with this object.
   */
  T*         get() const {
    return (_ptr);
  }

  /**
   *  Release the associated pointer and release it.
   *
   *  @return Pointer associated with this object.
   */
  T*         release() {
    T* tmp(_ptr);
    _ptr = NULL;
    return (tmp);
  }

  /**
   *  Reset this automatic pointer.
   *
   *  @param[in] ptr New pointer (can be NULL).
   */
  void       reset(T* t = NULL) {
    delete [] _ptr;
    _ptr = t;
    return ;
  }
};

CC_END()

#endif // !CC_UNIQUE_ARRAY_PTR_HH
