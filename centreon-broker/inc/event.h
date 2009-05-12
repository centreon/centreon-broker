/*
** event.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/11/09 Matthieu Kermagoret
*/

#ifndef EVENT_H_
# define EVENT_H_

# include "mutex.h"

namespace        CentreonBroker
{
  class          EventSubscriber;

  /**
   *  The Event class represents an event generated in Nagios and then
   *  forwarded to CentreonBroker. Event is just an interface that concrete
   *  implementations (like HostStatusEvent or ServiceStatusEvent) have to
   *  follow. In order to provide its whole content, a concrete Event have to
   *  use the Visitor design pattern ; first the Event accept the visitor and
   *  call its overloaded Visit() method with all components it would like to
   *  expose.
   *
   *  Currently all concrete Events have to be dynamically allocated with new,
   *  because when nobody is reading the Event anymore, it self-destructs.
   */
  class          Event
  {
   private:
    Mutex        mutex_;
    int          readers_;

   public:
                 Event();
                 Event(const Event& event);
    virtual      ~Event();
    Event&       operator=(const Event& event);
    virtual void AcceptVisitor(EventSubscriber& ev) = 0;
    void         AddReader(EventSubscriber* es);
    virtual int  GetType() const = 0;
    void         RemoveReader(EventSubscriber* es);
  };
}

#endif /* !EVENT_H_ */
