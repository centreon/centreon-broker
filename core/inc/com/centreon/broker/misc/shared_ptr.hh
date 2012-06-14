/*
** Copyright 2012 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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

  public:
    /**
     *  Construct shared pointer from a standalone pointer.
     *
     *  @param[in] ptr Pointer.
     */
                      shared_ptr(T* ptr = NULL) {
      if (ptr) {
        _refs_mtx = new QMutex;
        try {
          _refs = new unsigned int;
        }
        catch (...) {
          delete _refs_mtx;
          throw ;
        }
        *_refs = 1;
        _ptr = ptr;
      }
      else {
        _ptr = NULL;
        _refs = NULL;
        _refs_mtx = NULL;
      }
    }

    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
                      shared_ptr(shared_ptr<T> const& right) {
      // Copy data.
      QMutexLocker lock(&right._mtx);
      _internal_copy(right);

      // Increase reference count.
      if (_ptr) {
        QMutexLocker ref_lock(_refs_mtx);
        ++*_refs;
      }
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
      if (this != &right) {
        // Clear self.
        clear();

        // Copy shared pointer.
        QMutexLocker left_lock(&_mtx);
        QMutexLocker right_lock(&right._mtx);
        _internal_copy(right);

        // Increase reference count.
        if (_ptr) {
          QMutexLocker ref_lock(_refs_mtx);
          ++*_refs;
        }
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
      QMutexLocker lock(&_mtx);

      // Decrease reference count.
      if (_ptr) {
        QMutexLocker ref_lock(_refs_mtx);
        --*_refs;

        // No more reference, destroy everything.
        if (_refs <= 0) {
          ref_lock.unlock();
          delete _ptr;
          delete _refs;
          delete _refs_mtx;
        }

        // Reset pointers.
        _ptr = NULL;
        _refs = NULL;
        _refs_mtx = NULL;
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
     *  Cast a shared pointer to another type of shared pointer, sharing
     *  the same reference count.
     *
     *  @return Shared pointer of another type.
     */
    template          <typename U>
    shared_ptr<U>     staticCast() const {
      shared_ptr<U> retval;
      QMutexLocker lock(&_mtx);
      if (_ptr) {
        // Copy data.
        retval._ptr = static_cast<U*>(_ptr);
        retval._refs = _refs;
        retval._refs_mtx = _refs_mtx;

        // Increase reference count.
        QMutexLocker refs_lock(retval._refs_mtx);
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
    void              _internal_copy(shared_ptr<T> const& right) {
      _ptr = right._ptr;
      _refs = right._refs;
      _refs_mtx = right._refs_mtx;
      return ;
    }

    mutable QMutex    _mtx;
    T*                _ptr;
    unsigned int*     _refs;
    QMutex*           _refs_mtx;
  };
}

CCB_END()

#endif // !CCB_MISC_SHARED_PTR_HH
