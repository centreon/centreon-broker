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

# include <boost/asio.hpp>
# include <cstdio>
# include <string>
# include "thread.h"

namespace         CentreonBroker
{
  /**
   *  The NetworkInput class treats data coming from a client and parse it to
   *  generate appropriate Events.
   */
  class                           NetworkInput : private Thread
  {
   private:
    int                           fd_;
    boost::asio::ip::tcp::socket& socket_;
    std::string                   instance_;
                                  NetworkInput(const NetworkInput& ni);
    NetworkInput&                 operator=(const NetworkInput& ni);
    int                           Core();
    void                          HandleHostStatus(FILE* stream);

   public:
                                  NetworkInput(boost::asio::ip::tcp::socket&);
                                  ~NetworkInput();
  };
}

#endif /* !NETWORK_INPUT_H_ */
