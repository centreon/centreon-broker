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
     *  @class ref count
     *
     *  @brief  This class englobes the mutex and the refcount
     *          so as to reduce unnecessary allocations
     **/
    class ref_block{
    public:
      ref_block(): _refs(1){}
      int add_ref() {  QMutexLocker scope_lock(&_mtx);return ++_refs;}
      int release() {  QMutexLocker scope_lock(&_mtx);return --_refs;}
    private:
      QMutex    _mtx;
      int       _refs;

    };


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





    void swap(shared_ptr& left, shared_ptr& right){
      std::swap( left._ptr, right._ptr );
      std::swap( left._ref_block, right._ref_block );
    }

  public:
    /**
     *  Construct shared pointer from a standalone pointer.
     *
     *  @param[in] ptr Pointer.
     */
    shared_ptr(T* ptr = NULL)
      :  _ref_block(ptr? new ref_block() : 0),
         _ptr(ptr){}

    /**
     *  Copy constructor.
     *
     *  @param[in] right Object to copy.
     */
    shared_ptr(shared_ptr const& right)
    :  _ptr ( right._ptr),
       _ref_block( right._ref_block){

      // Increase reference count.
      if (_ref_block) {
        _ref_block->add_ref();
      }
    }



    /**
     *  Copy constructor for derived-typed smartpointers
     *
     *  @param[in] right Object to copy.
     */
    template < typename derived>
    shared_ptr(shared_ptr<derived> const& right)
    :  _ptr ( right._ptr),
       _ref_block( right._ref_block){

      // Increase reference count.
      if (_ref_block) {
        _ref_block->add_ref();
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
    shared_ptr<T>&    operator=(shared_ptr const& right) {
      shared_ptr<T> temp(right);
      swap(*this,temp);
      return (*this);
    }

    /**
     *  Copy another shared derived-type pointer.
     *
     *  @param[in] right Object to copy.
     *
     *  @return This object.
     */
    template < typename derived>
    shared_ptr<T>&    operator=(shared_ptr<derived> const& right) {

      // Technical Note :  clearly we do not need to check that RIGHT is a different object..
      //                   .. RIGHT is already a different type !
      clear();
      _ptr       = right._ptr;
      _ref_block = right._ref_block;
      if ( _ref_block)
        _ref_block->add_ref();
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
    void clear() {
      // Decrease reference count.
      if (_ref_block) {
        if ( _ref_block->release() == 0 ){
          delete _ptr;
          delete _ref_block;
          _ptr       = NULL;
          _ref_block = NULL;
        }
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
     *  [WARNING] .. a static_cast will allow a DOWNCAST to a derived type with NO checking !
     *            Is this a wise thing to do ?.. This "ought" to have been a dynamic_cast
     *            ...ONLY do this when you statically KNOW this is the *right thing*
     *            Good luck   ;-)
     *
     *  @return Shared pointer of another type.
     */
    template          <typename U>
    shared_ptr<U>     staticCast() const {

      //      _ptr = p;
      shared_ptr<U> retval;
      if (_ref_block) {
        // Copy data.
        retval._ref_block = _ref_block;
        retval._ptr = static_cast<U*>(_ptr);

        // Increase reference count.
        retval._ref_block->add_ref();
      }
      return (retval);
    }

  private:

    T*                _ptr;
    ref_block*        _ref_block;

  };
}

CCB_END()

#endif // !CCB_MISC_SHARED_PTR_HH
