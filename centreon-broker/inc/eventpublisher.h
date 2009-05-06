/*
** eventpublisher.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/06/09 Matthieu Kermagoret
*/

#ifndef EVENTPUBLISHER_H_
# define EVENTPUBLISHER_H_

# include <list>
# include "exception.h"
# include "mutex.h"

namespace                        CentreonBroker
{
  class                          EventSubscriber;

  class                          EventPublisher
  {
   private:
    std::list<EventSubscriber*>  subscribers;
    static EventPublisher*       instance;
    static CentreonBroker::Mutex mutex;
                                 EventPublisher();
                                 EventPublisher(const EventPublisher& ep);
    EventPublisher&              operator=(const EventPublisher& ep);

   public:
                                 ~EventPublisher();
    static EventPublisher*       GetInstance() throw (Exception);
    void                         Subscribe(EventSubscriber* es)
      throw (Exception);
    void                         Unsubscribe(EventSubscriber* es);
  };
}

#endif /* !EVENTPUBLISHER_H_ */
