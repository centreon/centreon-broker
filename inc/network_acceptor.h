/*
** network_acceptor.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/18/09 Matthieu Kermagoret
** Last update 05/19/09 Matthieu Kermagoret
*/

#ifndef NETWORK_ACCEPTOR_H_
# define NETWORK_ACCEPTOR_H_

# include <boost/asio.hpp>
# include <boost/system/system_error.hpp>
# include <list>
# include "exception.h"

namespace                          CentreonBroker
{
  // Forward declaration.
  class                            NetworkInput;

  /**
   *  The NetworkAcceptor class is responsible for accepting incoming clients.
   */
  class                            NetworkAcceptor
  {
   private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_service&       io_service_;
    boost::asio::ip::tcp::socket*  new_socket_;
    std::list<NetworkInput*>       ni_;
                                   NetworkAcceptor(const NetworkAcceptor& na)
                                     throw ();
    NetworkAcceptor&               operator=(const NetworkAcceptor& na)
                                     throw ();
    void                           StartAccept() throw ();

   public:
                                   NetworkAcceptor(boost::asio::io_service& i)
                                     throw ();
                                   ~NetworkAcceptor() throw ();
    void                           Accept(unsigned short port) throw (Exception);
    void                           HandleAccept(
                                     const boost::system::error_code& ec)
                                     throw ();
  };
}

#endif /* !NETWORK_ACCEPTOR_H_ */
