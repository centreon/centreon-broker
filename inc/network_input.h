/*
** network_input.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 06/18/09 Matthieu Kermagoret
*/

#ifndef NETWORK_INPUT_H_
# define NETWORK_INPUT_H_

# include <boost/asio.hpp>
# ifdef USE_TLS
#  include <boost/asio/ssl.hpp>
# endif /* USE_TLS */
# include <boost/thread.hpp>
# include <cstddef>
# include <memory>
# include <string>
# include "connection_status.h"
# include "protocol_socket.h"

namespace                         CentreonBroker
{
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
    std::auto_ptr<ProtocolSocket> socket_;
    boost::thread*                thread_;
                                  NetworkInput(boost::asio::ip::tcp::socket*);
# ifdef USE_TLS
				  NetworkInput(boost::asio::ssl::stream<
				    boost::asio::ip::tcp::socket>*);
# endif /* USE_TLS */
                                  NetworkInput(const NetworkInput& ni);
    NetworkInput&                 operator=(const NetworkInput& ni);
    void                          HandleAcknowledgement(ProtocolSocket& ps);
    void                          HandleComment(ProtocolSocket& ps);
    void                          HandleHost(ProtocolSocket& ps);
    void                          HandleHostStatus(ProtocolSocket& ps);
    void                          HandleInitialization(ProtocolSocket& ps);
    void                          HandleProgramStatus(ProtocolSocket& ps);
    void                          HandleService(ProtocolSocket& ps);
    void                          HandleServiceStatus(ProtocolSocket& ps);
# ifdef USE_TLS
    void                          Handshake(const boost::system::error_code&);
# endif /* USE_TLS */

   public:
                                  ~NetworkInput() throw ();
    void                          operator()();
  };
}

#endif /* !NETWORK_INPUT_H_ */
