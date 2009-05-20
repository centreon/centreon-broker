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
# include <boost/thread.hpp>
# include <cstdio>
# include <string>

namespace         CentreonBroker
{
  /**
   *  The NetworkInput class treats data coming from a client and parse it to
   *  generate appropriate Events.
   */
  class                           NetworkInput
  {
   private:
    int                           fd_;
    std::string                   instance_;
    boost::asio::ip::tcp::socket& socket_;
    boost::thread*                thread_;
                                  NetworkInput(const NetworkInput& ni);
    NetworkInput&                 operator=(const NetworkInput& ni);
    void                          HandleHostStatus(FILE* stream);

   public:
                                  NetworkInput(boost::asio::ip::tcp::socket&);
                                  ~NetworkInput();
    void                          operator()();
  };
}

#endif /* !NETWORK_INPUT_H_ */
