/*
** Copyright 2012-2014 Centreon
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

#ifndef CCB_MISC_SHARED_PTR_HH
#  define CCB_MISC_SHARED_PTR_HH

#  include <QMutex>
#  include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace             misc {  
  /**
   *  @class shared_ptr shared_ptr.hh "com/centreon/broker/misc/shared_ptr.hh"
   *  @brief Shared pointer.
   *
   *  Explicitely hold shareable strong reference to some object.
   */
  template            <typename T>
  class               shared_ptr {
    template          <typename U>
    friend class      shared_ptr;
    template          <typename U>
    friend class      weak_ptr;

  public:
    /**
     *  Construct shared pointer from a standalone pointer.
     *
     *  @param[in] ptr Pointer.
     */
                      shared_ptr(T* ptr = NULL) {
      if (ptr) {
        _mtx = new QMutex;
        try {
          _refs = new unsigned int;
          _weak_refs = new unsigned int;
        }
        catch (...) {
          delete _mtx;
          throw ;
        }
        *_refs = 1;
        *_weak_refs = 0;
        _ptr = ptr;
      }
      else {
        _mtx = NULL;
        _ptr = NULL;
        _refs = NULL;
        _weak_refs = NULL;
      }
    }

    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
                      shared_ptr(shared_ptr<T> const& right) {
      _internal_copy(right);
    }

    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
    template <typename U>
                      shared_ptr(shared_ptr<U> const& right) {
      _internal_copy(right);
    }

    /**
     *  Destroy a shared pointer.
     */
                      ~shared_ptr() throw () {
      try {
        clear();
      }
      catch (...) {}
    }

    /**
    *  Copy another shared pointer.
    *
    *  @param[in] right Object to copy.
    *
    *  @return This object.
    */
    shared_ptr<T>&    operator=(shared_ptr<T> const& right) {
      if (_ptr != right._ptr) {
        clear();
        _internal_copy(right);
      }
      return (*this);
    }

    /**
     *  Copy another shared pointer.
     *
     *  @param[in] right Object to copy.
     *
     *  @return This object.
     */
    template <typename U>
    shared_ptr<T>&    operator=(shared_ptr<U> const& right) {
      if (_ptr != right._ptr) {
        clear();
        _internal_copy(right);
      }
      return (*this);
    }

    /**
     *  Evaluate to boolean.
     *
     *  @return true if pointer is not NULL.
     */
                      operator bool() const {
      return (_ptr);
    }

    /**
     *  Check if pointer is NULL.
     *
     *  @return true if pointer is NULL.
     *
     *  @see operator bool()
     */
    bool              operator!() const {
      return (!operator bool());
    }

    /**
     *  Get reference to pointer.
     *
     *  @return Reference to stored pointer.
     */
    T&                operator*() const {
      return (*_ptr);
    }

    /**
     *  Get pointer.
     *
     *  @return Pointer.
     */
    T*                operator->() const {
      return (_ptr);
    }

    /**
     *  Clear current pointer.
     */
    void              clear() {
      // Decrease reference count.
      if (_ptr) {
        QMutexLocker ref_lock(_mtx);
        --*_refs;

        // No more reference, destroy everything.
        if (*_refs <= 0) {
          if (*_weak_refs <= 0) {
            ref_lock.unlock();
            delete _mtx;
            delete _refs;
            delete _weak_refs;
          }
          ref_lock.unlock();
          delete _ptr;
        }

        // Reset pointers.
        _mtx = NULL;
        _ptr = NULL;
        _refs = NULL;
        _weak_refs = NULL;
      }
      return ;
    }

    /**
     *  Get pointer.
     *
     *  @return Pointer.
     */
    T*                data() const {
      return (_ptr);
    }

    /**
     *  Check is pointer is NULL.
     *
     *  @return true if pointer is NULL.
     */
    bool              isNull() const {
      return (!_ptr);
    }

    /**
     *  Get reference to pointer.
     *
     *  @return  Reference to pointer.
     */
    T&                ref() const {
      return (*_ptr);
    }

    /**
     *  Get reference as.
     *
     *  @return  Reference casted as.
     */
    template <typename U>
    U&                ref_as() const {
      return (static_cast<U&>(*_ptr));
    }

    /**
     *  Cast a shared pointer to another type of shared pointer, sharing
     *  the same reference count.
     *
     *  @return Shared pointer of another type.
     */
    template          <typename U>
    shared_ptr<U>     staticCast() const {
      shared_ptr<U> retval;
      if (_ptr) {
        // Copy data.
        retval._mtx = _mtx;
        retval._ptr = static_cast<U*>(_ptr);
        retval._refs = _refs;
        retval._weak_refs = _weak_refs;

        // Increase reference count.
        QMutexLocker refs_lock(retval._mtx);
        ++*retval._refs;
      }
      return (retval);
    }

  private:
    /**
     *  Copy internal data members.
     *
     *  @param[in] right Object to copy.
     */
    template <typename U>
    void              _internal_copy(shared_ptr<U> const& right) {
      // Copy data.
      _mtx = right._mtx;
      _ptr = right._ptr;
      _refs = right._refs;
      _weak_refs = right._weak_refs;

      // Increase reference count.
      if (_ptr) {
        QMutexLocker ref_lock(_mtx);
        ++*_refs;
      }

      return ;
    }

    QMutex*           _mtx;
    T*                _ptr;
    unsigned int*     _refs;
    unsigned int*     _weak_refs;
  };

  /**
   *  Create a shared pointer from an object.
   *
   *  @param[in] obj  The object.
   */
  template <typename T>
  misc::shared_ptr<T> make_shared(T* obj) {
    return (misc::shared_ptr<T>(obj));
  }
}

CCB_END()

#endif // !CCB_MISC_SHARED_PTR_HH
