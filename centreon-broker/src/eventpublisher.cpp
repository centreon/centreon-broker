/*
** eventpublisher.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/07/09 Matthieu Kermagoret
*/

#include <cassert>
#include <cstdlib>
#include "eventpublisher.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Static Members            *
*                                     *
**************************************/

EventPublisher* EventPublisher::instance = NULL;
Mutex           EventPublisher::mutex;

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
  delete (EventPublisher::GetInstance());
  return ;
}

/**
 *  EventPublisher destructor.
 */
EventPublisher::~EventPublisher()
{
}

/**
 *  Retrieve the instance of EventPublisher.
 */
EventPublisher* EventPublisher::GetInstance() throw (Exception)
{
  if (!EventPublisher::instance)
    {
      EventPublisher::mutex.Lock();
      if (!EventPublisher::instance)
	{
	  try
	    {
	      EventPublisher::instance = new (EventPublisher);
	      atexit(delete_eventpublisher);
	    }
	  catch (...)
	    {
	      EventPublisher::mutex.Unlock();
	      throw (Exception("Event publisher instantiation failed."));
	    }
	}
      EventPublisher::mutex.Unlock();
    }
  return (EventPublisher::instance);
}

/**
 *  Add an object that shall be warned when an event occur.
 */
void EventPublisher::Subscribe(EventSubscriber* es) throw (Exception)
{
  try
    {
      this->subscribers.push_front(es);
    }
  catch (...)
    {
      throw (Exception("Could not add a subscriber."));
    }
  return ;
}

/**
 *  Unsubscribe an object.
 */
void EventPublisher::Unsubscribe(EventSubscriber* es)
{
  std::list<EventSubscriber*>::iterator it;

  for (it = this->subscribers.begin(); it != this->subscribers.end(); it++)
    if ((*it) == es)
      {
	this->subscribers.erase(it);
	break ;
      }
  assert(it != this->subscribers.end());
  return ;
}
