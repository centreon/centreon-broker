/*
** Copyright 2009-2011 MERETHIS
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
**
** For more information: contact@centreon.com
*/

#ifndef IO_ACCEPTOR_HH_
# define IO_ACCEPTOR_HH_

namespace           io {
  // Forward declaration.
  class             stream;

  /**
   *  @class acceptor acceptor.hh "io/acceptor.hh"
   *  @brief Accept incoming clients.
   *
   *  An acceptor is a kind of 'gate' on which clients can come,
   *  eventually authentify, in order to generate a new session
   *  (represented by a stream object) and perform I/O operations with
   *  the application. It is not defined within the acceptor interface
   *  how the client connects to the acceptor or how or if it
   *  authenticates.
   *
   *  @see io::net::ipv4_acceptor
   *  @see io::net::ipv6_acceptor
   *  @see io::net::unix_acceptor
   *  @see stream
   */
  class             acceptor {
   protected:
                    acceptor();
                    acceptor(acceptor const& a);
    acceptor&       operator=(acceptor const& a);

   public:
    virtual         ~acceptor();
    virtual stream* accept() = 0;
    virtual void    close() = 0;
  };
}

#endif /* !IO_ACCEPTOR_HH_ */
