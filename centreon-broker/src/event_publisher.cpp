/*
** event_publisher.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstdlib>
#include "event.h"
#include "event_publisher.h"
#include "event_subscriber.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Static Members            *
*                                     *
**************************************/

EventPublisher* EventPublisher::instance_ = NULL;
boost::mutex    EventPublisher::instancem_;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  EventPublisher constructor.
 */
EventPublisher::EventPublisher()
{
}

/**
 *  EventPublisher copy constructor.
 */
EventPublisher::EventPublisher(const EventPublisher& ep)
{
  (void)ep;
}

/**
 *  EventPublisher operator= overload.
 */
EventPublisher& EventPublisher::operator=(const EventPublisher& ep)
{
  (void)ep;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  This function will be called on termination to free ressources used by the
 *  EventPublisher.
 */
static void delete_eventpublisher()
{
  assert(EventPublisher::GetInstance());
  delete (EventPublisher::GetInstance());
  return ;
}

/**
 *  EventPublisher destructor.
 */
EventPublisher::~EventPublisher()
{
  EventPublisher::instancem_.lock();
  EventPublisher::instance_ = NULL;
  EventPublisher::instancem_.unlock();
}

/**
 *  Retrieve the instance of EventPublisher.
 */
EventPublisher* EventPublisher::GetInstance()
{
  if (!EventPublisher::instance_)
    {
      EventPublisher::instancem_.lock();
      if (!EventPublisher::instance_)
	{
	  try
	    {
	      EventPublisher::instance_ = new (EventPublisher);
	    }
	  catch (...) // Do not let the mutex locked.
	    {
	      EventPublisher::instancem_.unlock();
	      throw ;
	    }
	  atexit(delete_eventpublisher);
	  EventPublisher::instancem_.unlock();
	}
    }
  return (EventPublisher::instance_);
}

/**
 *  Sends an event to all subscribers.
 */
void EventPublisher::Publish(Event* ev)
{
  std::list<EventSubscriber*>::iterator it;
  boost::unique_lock<boost::mutex> lock(this->subscribersm_);

  for (it = this->subscribers_.begin(); it != this->subscribers_.end(); it++)
    ev->AddReader(*it);
  for (it = this->subscribers_.begin(); it != this->subscribers_.end(); it++)
    (*it)->OnEvent(ev);
  return ;
}

/**
 *  Add an object that shall be warned when an event occur.
 */
void EventPublisher::Subscribe(EventSubscriber* es)
{
  boost::unique_lock<boost::mutex> lock(this->subscribersm_);

  this->subscribers_.push_front(es);
  return ;
}

/**
 *  Unsubscribe an object.
 */
void EventPublisher::Unsubscribe(EventSubscriber* es)
{
  boost::unique_lock<boost::mutex> lock(this->subscribersm_);

  this->subscribers_.remove(es);
  return ;
}
