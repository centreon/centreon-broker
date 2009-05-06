/*
** eventsubscriber.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/06/09 Matthieu Kermagoret
*/

#ifndef EVENTSUBSCRIBER_H_
# define EVENTSUBSCRIBER_H_

# include "exception.h"

namespace            CentreonBroker
{
  class              Event;

  class              EventSubscriber
  {
   public:
                     EventSubscriber() throw (Exception);
                     EventSubscriber(const EventSubscriber& es)
      throw (Exception);
    virtual          ~EventSubscriber();
    EventSubscriber& operator=(const EventSubscriber& es);
    virtual void     OnDump(Event* e, ...) = 0;
    virtual void     OnEvent(Event* e) = 0;
  };
}

#endif /* !EVENTSUBSCRIBER_H_ */
