/*
** network_input.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 06/16/09 Matthieu Kermagoret
*/

#ifndef NETWORK_INPUT_H_
# define NETWORK_INPUT_H_

# include <boost/asio.hpp>
# include <boost/thread.hpp>
# include <cstddef>
# include <string>
# include "connection_status.h"

namespace                         CentreonBroker
{
  class                           ProtocolSocket;

  /**
   *  The NetworkInput class treats data coming from a client and parse it to
   *  generate appropriate Events.
   */
  class                           NetworkInput
  {
    friend class                  NetworkAcceptor;

   private:
    ConnectionStatus              conn_status_;
    std::string                   instance_;
    boost::asio::ip::tcp::socket& socket_;
    boost::thread*                thread_;
                                  NetworkInput(boost::asio::ip::tcp::socket&);
                                  NetworkInput(const NetworkInput& ni);
    NetworkInput&                 operator=(const NetworkInput& ni);
    void                          HandleAcknowledgement(ProtocolSocket& ps);
    void                          HandleHost(ProtocolSocket& ps);
    void                          HandleHostStatus(ProtocolSocket& ps);
    void                          HandleInitialization(ProtocolSocket& ps);
    void                          HandleProgramStatus(ProtocolSocket& ps);
    void                          HandleService(ProtocolSocket& ps);
    void                          HandleServiceStatus(ProtocolSocket& ps);

   public:
                                  ~NetworkInput() throw ();
    void                          operator()();
  };
}

#endif /* !NETWORK_INPUT_H_ */
