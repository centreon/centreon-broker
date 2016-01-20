/*
** Copyright 2016 Centreon
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

#ifndef CCB_MISC_WEAK_PTR_HH
#  define CCB_MISC_WEAK_PTR_HH

#  include <QMutex>
#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/misc/shared_ptr.hh"

CCB_BEGIN()

namespace             misc {
  /**
   *  @class weak_ptr weak_ptr.hh "com/centreon/broker/misc/weak_ptr.hh"
   *  @brief Weak shared pointer.
   *
   *  Explicitely hold shareable weak reference to some object.
   */
  template            <typename T>
  class               weak_ptr {
    template          <typename U>
    friend class      weak_ptr;

  public:
    /**
     *  Construct weakly shared pointer from shared ptr.
     *
     *  @param[in] ptr Pointer.
     */
                      weak_ptr(misc::shared_ptr<T> ptr = misc::shared_ptr<T>()) {
      if (!ptr.isNull()) {
        _mtx = ptr._mtx;
        _ptr = ptr._ptr;
        _refs = ptr._refs;
        _weak_refs = ptr._weak_refs;
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
                      weak_ptr(weak_ptr<T> const& right) {
      _internal_copy(right);
    }

    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
    template <typename U>
                      weak_ptr(weak_ptr<U> const& right) {
      _internal_copy(right);
    }

    /**
     *  Destroy a shared pointer.
     */
                      ~weak_ptr() throw () {
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
    weak_ptr<T>&    operator=(weak_ptr<T> const& right) {
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
    weak_ptr<T>&    operator=(weak_ptr<U> const& right) {
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
      if (_weak_refs) {
        QMutexLocker ref_lock(_mtx);
        if (*_refs == 0) {
          ref_lock.unlock();
          clear();
        }
        else
          return (true);
      }
      return (false);
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
     *  Get a shared ptr from a weak ptr.
     *
     *  @return  The shared ptr.
     */
    misc::shared_ptr<T>
                      lock() {
      if (_weak_refs) {
        QMutexLocker ref_lock(_mtx);

        // No hard reference left, clear and return an empty shared ptr.
        if (*_refs == 0) {
          ref_lock.unlock();
          clear();
        }
        else {
          // Create a new shared_ptr and add one to the hard reference count.
          misc::shared_ptr<T> retval;
          retval._mtx = _mtx;
          retval._ptr = _ptr;
          retval._refs = _refs;
          retval._weak_refs = _weak_refs;
          ++*_refs;
          ref_lock.unlock();
          return (retval);
        }
      }

      return (misc::shared_ptr<T>());
    }

    /**
     *  Clear current pointer.
     */
    void              clear() {
      // Decrease reference count.
      if (_weak_refs) {
        QMutexLocker ref_lock(_mtx);

        --*_weak_refs;
        if (*_weak_refs <= 0 && *_refs <= 0) {
          ref_lock.unlock();
          delete _mtx;
          delete _refs;
          delete _weak_refs;
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
     *  Check is pointer is NULL.
     *
     *  @return true if pointer is NULL.
     */
    bool              isNull() const {
      return ((bool)(*this));
    }

    /**
     *  Cast a shared pointer to another type of shared pointer, sharing
     *  the same reference count.
     *
     *  @return Shared pointer of another type.
     */
    template          <typename U>
    weak_ptr<U>     staticCast() const {
      shared_ptr<U> retval;
      if (_ptr) {
        // Copy data.
        retval._mtx = _mtx;
        retval._ptr = static_cast<U*>(_ptr);
        retval._refs = _refs;
        retval._weak_refs = _weak_refs;

        // Increase reference count.
        QMutexLocker refs_lock(retval._mtx);
        ++*retval._weak_refs;
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
    void              _internal_copy(weak_ptr<U> const& right) {
      // Copy data.
      _mtx = right._mtx;
      _ptr = right._ptr;
      _refs = right._refs;
      _weak_refs = right._weak_refs;

      // Increase reference count.
      if (_ptr) {
        QMutexLocker ref_lock(_mtx);
        ++*_weak_refs;
      }

      return ;
    }

    QMutex*           _mtx;
    T*                _ptr;
    unsigned int*     _refs;
    unsigned int*     _weak_refs;
  };
}

CCB_END()

#endif // !CCB_MISC_WEAK_PTR_HH
