/*
** eventsubscriber.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/13/09 Matthieu Kermagoret
*/

#ifndef EVENTSUBSCRIBER_H_
# define EVENTSUBSCRIBER_H_

# include <string>
# include <sys/types.h>

namespace            CentreonBroker
{
  class              Event;

  /**
   *  An EventSubscriber can subscribe against the EventPublisher to receive
   *  events when they occur. It can then access data from the Event using the
   *  Visitor pattern : the subscriber call the AcceptVisitor() method of the
   *  Event which will in return call the overloaded Visit() method.
   */
  class              EventSubscriber
  {
   public:
                     EventSubscriber();
                     EventSubscriber(const EventSubscriber& es);
    virtual          ~EventSubscriber();
    EventSubscriber& operator=(const EventSubscriber& es);
    virtual void     OnEvent(Event* e) = 0;
  };
}

#endif /* !EVENTSUBSCRIBER_H_ */
