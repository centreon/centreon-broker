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

#ifndef CLIENT_ACCEPTOR_H_
# define CLIENT_ACCEPTOR_H_

# include <boost/thread.hpp>
# include "io/io.h"
# include "network_input.h"

namespace                        CentreonBroker
{
  /**
   *  \class ClientAcceptor client_acceptor.h "client_acceptor.h"
   *  \brief Wait for incoming clients and then launch processing thread.
   *
   *  A ClientAcceptor will wait for incoming clients on a provided acceptor.
   *  When a new client arrives, the ClientAcceptor class has the
   *  responsability to create the thread that will process the input of this
   *  client (namely a NetworkInput object). Upon ClientAcceptor termination,
   *  it will close all previously launched threads.
   *
   *  \see NetworkInput
   *  \see Acceptor
   */
  class                          ClientAcceptor
  {
   private:
    std::auto_ptr<IO::Acceptor>  acceptor_;
    std::list<NetworkInput*>     inputs_;
    boost::mutex                 inputsm_;
    std::auto_ptr<boost::thread> thread_;
                                 ClientAcceptor(const ClientAcceptor& ca);
    ClientAcceptor&              operator=(const ClientAcceptor& ca);

   public:
                                 ClientAcceptor() throw ();
                                 ~ClientAcceptor();
    void                         operator()() throw ();
    void                         CleanupNetworkInput(NetworkInput* ni);
    void                         Run(IO::Acceptor* acceptor);
  };
}

#endif /* !CLIENT_ACCEPTOR_H_ */
