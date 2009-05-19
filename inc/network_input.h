/*
** network_input.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef NETWORK_INPUT_H_
# define NETWORK_INPUT_H_

# include <cstdio>
# include <string>
# include "thread.h"

namespace         CentreonBroker
{
  class           NetworkInput : private Thread
  {
   private:
    int           fd_;
    std::string   instance_;
                  NetworkInput(const NetworkInput& ni);
    NetworkInput& operator=(const NetworkInput& ni);
    int           Core();
    void          HandleHostStatus(FILE* stream);

   public:
                  NetworkInput();
                  ~NetworkInput();
    void          SetFD(int fd);
  };
}

#endif /* !NETWORK_INPUT_H_ */
