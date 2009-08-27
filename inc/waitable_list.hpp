/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef WAITABLE_LIST_HPP_
# define WAITABLE_LIST_HPP_

# include <boost/thread.hpp>
# include <list>
# include "exception.h"
# include "logging.h"

namespace                     CentreonBroker
{
  /**
   *  \class WaitableList waitable_list.hpp "waitable_list.hpp"
   *  \brief List of objects on which one can wait for it to be filled.
   *
   *  This class is similar to an std::list with the exception that if the list
   *  is empty, one can wait for the list to be filled. All locking/notifying
   *  process is handled internally.
   *
   *  \tparam T Base type of the list. The list will hold pointers to such type
   *            because of a C++/gcc limitation.
   */
  template <typename T>
  class                       WaitableList
  {
    /**********************************
    *                                 *
    *         Private Members         *
    *                                 *
    **********************************/
   private:
    boost::condition_variable cv_;
    std::list<T*>             list_;
    boost::mutex              mutex_;

    /**
     *  \brief Copy the list given as a parameter within the current instance.
     *
     *  Copy the list stored within the WaitableList parameter within the
     *  current instance. This method is just by the copy constructor and the
     *  assignement operator.
     *
     *  \param wl List to copy data from.
     *
     *  \see WaitableList
     *  \see operator=
     */
    void                      InternalCopy(const WaitableList& wl)
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      this->list_ = wl.list_;
      return ;
    }

    /**********************************
    *                                 *
    *         Public Members          *
    *                                 *
    **********************************/
   public:
    /**
     *  \brief WaitableList default constructor.
     *
     *  Build an empty list.
     */
                              WaitableList() {}

    /**
     *  \brief WaitableList copy constructor.
     *
     *  Copy the list stored within the given WaitableList to the current
     *  instance.
     *
     *  \param wl List to copy data from.
     */
                              WaitableList(const WaitableList& wl)
    {
      this->InternalCopy(wl);
    }

    /**
     *  \brief Waitable List destructor.
     *
     *  Destroy the list.
     */
                              ~WaitableList() {}

    /**
     *  \brief Overload of the assignement operator.
     *
     *  Copy the list stored within the given WaitableList to the current
     *  instance.
     *
     *  \param wl List to copy data from.
     *
     *  \return *this
     */
    WaitableList&             operator=(const WaitableList& wl)
    {
      this->InternalCopy(wl);
      return (*this);
    }

    /**
     *  \brief Add an element to the list.
     *
     *  Store the given element at the end of the list.
     *
     *  \param t Object to store in the list.
     */
    void                      Add(T* t)
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      this->list_.push_back(t);
      this->cv_.notify_all();
      return ;
    }

    /**
     *  \brief Resume threads waiting on the list.
     *
     *  Resume all threads blocked by a Wait() or TimedWait() call.
     */
    void                      CancelWait() throw ()
    {
#ifndef NDEBUG
      logging.LogDebug("Resume all thread Wait()ing...");
#endif /* !NDEBUG */
      this->cv_.notify_all();
      return ;
    }

    /**
     *  \brief Determines whether or not the list is empty.
     *
     *  \return true if the list is empty, false otherwise.
     */
    bool                      Empty()
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      return (this->list_.empty());
    }

    /**
     *  Get the internal list of objects.
     *
     *  \return The internal list of objects.
     */
    std::list<T*>&            List()
    {
      return (this->list_);
    }

    /**
     *  \brief Lock the internal mutex.
     *
     *  This method can be useful when directly accessing the internal list.
     */
    void                      Lock()
    {
      this->mutex_.lock();
      return ;
    }

    /**
     *  \brief Wait until an element is received or until the timeout occur.
     *
     *  Wait on the list until an element is received through Add() or until
     *  the timeout occur (whichever comes first). This call can be cancelled
     *  via CancelWait().
     *
     *  \param st Timeout.
     *
     *  \return The new element in the list, NULL if the timeout occured.
     */
    T*                        TimedWait(const boost::system_time& st)
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);
      T* t;

      if (this->list_.empty())
	{
	  this->cv_.timed_wait(lock, st);
	}
      if (this->list_.empty())
	t = NULL;
      else
	{
	  t = this->list_.front();
	  this->list_.pop_front();
	}
      return (t);
    }

    /**
     *  \brief Unlock the mutex.
     *
     *  Unlock the internal mutex previously locked by a call to Lock().
     *
     *  \see Lock
     */
    void                      Unlock()
    {
      this->mutex_.unlock();
      return ;
    }

    /**
     *  \brief Wait until an element is received.
     *
     *  Wait on the list until an element is received through Add(). This call
     *  can be cancelled via CancelWait().
     *
     *  \return The new element in the list.
     */
    T*                        Wait()
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);
      T* t;

      if (this->list_.empty())
	{
	  this->cv_.wait(lock);
	}
      if (this->list_.empty())
	throw (Exception(ECANCELED, "Spurious condition variable wake-up."));
      else
	{
	  t = this->list_.front();
	  this->list_.pop_front();
	}
      return (t);
    }
  };
}

#endif /* !WAITABLE_LIST_HPP_ */
