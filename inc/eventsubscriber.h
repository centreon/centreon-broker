/*
** eventsubscriber.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/06/09 Matthieu Kermagoret
** Last update 05/07/09 Matthieu Kermagoret
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
    virtual void     OnEvent(Event* e) = 0;
    virtual void     Visit(const char* arg) = 0;
    virtual void     Visit(double arg) = 0;
    virtual void     Visit(int arg) = 0;
    virtual void     Visit(short arg) = 0;
    virtual void     Visit(const std::string& arg) = 0;
    virtual void     Visit(time_t arg) = 0;
  };
}

#endif /* !EVENTSUBSCRIBER_H_ */
