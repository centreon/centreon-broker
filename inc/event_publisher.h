/*
** event_publisher.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef EVENT_PUBLISHER_H_
# define EVENT_PUBLISHER_H_

# include <list>
# include "mutex.h"

namespace                       CentreonBroker
{
  class                         Event;
  class                         EventSubscriber;

  /**
   *  The EventPublisher is a singleton that is responsible of broadcasting
   *  events to every EventSubscriber who asked it. The EventPublisher first
   *  receive events via Publish() (usually called from NetworkInput) and then
   *  sends it to every object which Subscribe()'d.
   */
  class                         EventPublisher
  {
   private:
    std::list<EventSubscriber*> subscribers_;
    Mutex                       subscribersm_;
    static EventPublisher*      instance_;
    static Mutex                instancem_;
                                EventPublisher();
                                EventPublisher(const
                                               EventPublisher& ep);
    EventPublisher&             operator=(const EventPublisher& ep);

   public:
                                ~EventPublisher();
    static EventPublisher*      GetInstance();
    void                        Publish(Event* ev);
    void                        Subscribe(EventSubscriber* es);
    void                        Unsubscribe(EventSubscriber* es);
  };
}

#endif /* !EVENT_PUBLISHER_H_ */
