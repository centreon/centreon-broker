/*
** networkinput.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/11/09 Matthieu Kermagoret
*/

#ifndef NETWORKINPUT_H_
# define NETWORKINPUT_H_

# include "thread.h"

namespace         CentreonBroker
{
  class           NetworkInput : private Thread
  {
   private:
    int           fd;
                  NetworkInput(const NetworkInput& ni);
    NetworkInput& operator=(const NetworkInput& ni);
    int           Core();

   public:
                  NetworkInput();
                  ~NetworkInput();
    void          SetFD(int fd);
  };
}

#endif /* !NETWORKINPUT_H_ */
