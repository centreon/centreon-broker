/*
** event.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/04/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef EVENT_H_
# define EVENT_H_

# include <boost/thread/mutex.hpp>
# include <string>

namespace              CentreonBroker
{
  class                EventSubscriber;

  /**
   *  The Event class represents an event generated in Nagios and then
   *  forwarded to CentreonBroker. Event is just an interface that concrete
   *  implementations (like HostStatusEvent or ServiceStatusEvent) have to
   *  follow.
   *
   *  Currently all concrete Events have to be dynamically allocated with new,
   *  because when nobody is reading the Event anymore, it self-destructs.
   */
  class                Event
  {
   private:
    boost::mutex       mutex_;
    std::string        nagios_instance_;
    int                readers_;

   public:
                       Event();
                       Event(const Event& event);
    virtual            ~Event();
    Event&             operator=(const Event& event);
    void               AddReader(EventSubscriber* es);
    const std::string& GetNagiosInstance() const throw ();
    virtual int        GetType() const = 0;
    void               RemoveReader(EventSubscriber* es);
    void               SetNagiosInstance(const std::string& inst);
  };
}

#endif /* !EVENT_H_ */
