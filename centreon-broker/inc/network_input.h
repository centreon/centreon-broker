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
# include "events/connection_status.h"
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
    Events::ConnectionStatus      conn_status_;
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
    void                          HandleDowntime(ProtocolSocket& ps);
    void                          HandleHost(ProtocolSocket& ps);
    void                          HandleHostGroup(ProtocolSocket& ps);
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
