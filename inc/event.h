/*
** event.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/04/09 Matthieu Kermagoret
*/

#ifndef EVENT_H_
# define EVENT_H_

namespace   CentreonBroker
{
  class     Event
  {
   private:
            Event(const Event& event);
    Event&  operator=(const Event& event);

   public:
            Event();
    virtual ~Event();
  };
}

#endif /* !EVENT_H_ */
