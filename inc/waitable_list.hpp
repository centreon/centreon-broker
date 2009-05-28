/*
** waitable_list.hpp for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/26/09 Matthieu Kermagoret
** Last update 05/26/09 Matthieu Kermagoret
*/

#ifndef WAITABLE_LIST_HPP_
# define WAITABLE_LIST_HPP_

# include <boost/thread.hpp>
# include <list>

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
    std::list<T>              list_;
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
    void                      Add(T t)
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);

      this->list_.push_back(t);
      return ;
    }

    /**
     *  Resume all threads blocked by a Wait() or TimedWait() call.
     */
    void                      CancelWait() throw ()
    {
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
    T                         Wait()
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);
      T t;

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
    T                         TimedWait(const boost::system_time& st)
    {
      boost::unique_lock<boost::mutex> lock(this->mutex_);
      T t;

      if (this->list_.empty())
	{
	  this->cv_.timed_wait(lock, st);
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
