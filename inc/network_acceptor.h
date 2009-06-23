/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#ifndef NETWORK_ACCEPTOR_H_
# define NETWORK_ACCEPTOR_H_

# include <boost/asio.hpp>
# ifdef USE_TLS
#  include <boost/asio/ssl.hpp>
# endif /* USE_TLS */
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
    boost::asio::ip::tcp::socket*  new_normal_socket_;
# ifdef USE_TLS
    std::string                    ca_;
    std::string                    cert_;
    std::string                    key_;
    bool                           tls_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* new_tls_socket_;
    boost::asio::ssl::context      context_;
# endif /* USE_TLS */
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
# ifdef USE_TLS
    void                           SetTls(const std::string& certificate = "",
					  const std::string& key = "",
					  const std::string& ca = "");
# endif /* USE_TLS */
  };
}

#endif /* !NETWORK_ACCEPTOR_H_ */
