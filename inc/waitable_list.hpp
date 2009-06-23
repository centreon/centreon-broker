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
     *  Copy all internal data of the WaitableList object to the current
     *  instance.
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
     *  WaitableList default constructor.
     */
                              WaitableList()
    {
    }
    /**
     *  WaitableList copy constructor.
     */
                              WaitableList(const WaitableList& wl)
    {
      this->InternalCopy(wl);
    }

    /**
     *  Waitable List destructor.
     */
                              ~WaitableList()
    {
    }

    /**
     *  WaitableList operator= overload.
     */
    WaitableList&             operator=(const WaitableList& wl)
    {
      this->InternalCopy(wl);
      return (*this);
    }

    /**
     *  Add an element in the list.
     */
    void                      Add(T* t)
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      this->list_.push_back(t);
      this->cv_.notify_all();
      return ;
    }

    /**
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
     *  Determines whether or not the list is empty.
     */
    bool                      Empty()
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      return (this->list_.empty());
    }

    /**
     *  Wait until an element is received.
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

    /**
     *  Wait until an element is received or until the timeout occur.
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
  };
}

#endif /* !WAITABLE_LIST_HPP_ */
