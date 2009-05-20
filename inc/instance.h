/*
** instance.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/20/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#ifndef INSTANCE_H_
# define INSTANCE_H_

# include <string>
# include "event.h"

namespace       CentreonBroker
{
  class         Instance : public Event
  {
   private:
    enum        String
    {
      ADDRESS = 0,
      DESCRIPTION,
      NAME,
      STRING_NB
    };
    std::string strings_[STRING_NB];
    void        InternalCopy(const Instance& inst);

  public:
                Instance();
                Instance(const Instance& inst);
                ~Instance();
    Instance&   operator=(const Instance& inst);
    int         GetType() const throw ();
  };
}

#endif /* !INSTANCE_H_ */
