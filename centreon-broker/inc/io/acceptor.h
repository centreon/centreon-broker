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

#ifndef IO_ACCEPTOR_H_
# define IO_ACCEPTOR_H_

namespace           IO
{
  // Forward declaration.
  class             Stream;

  /**
   *  \class Acceptor acceptor.h "io/acceptor.h"
   *  \brief Accept incoming clients.
   *
   *  An Acceptor is a kind of 'gate' on which clients can come, eventually
   *  authentify, in order to generate a new session (represented by a Stream
   *  object) and perform I/O operations with the application. It is not
   *  defined within the Acceptor interface how the client connects to the
   *  acceptor or how or if it authenticates.
   *
   *  \see IO::Net::IPv4Acceptor
   *  \see IO::Net::IPv6Acceptor
   *  \see IO::Net::UnixAcceptor
   *  \see Stream
   */
  class             Acceptor
  {
   protected:
                    Acceptor();
                    Acceptor(const Acceptor& acceptor);
    Acceptor&       operator=(const Acceptor& acceptor);

   public:
    virtual         ~Acceptor();
    virtual Stream* Accept() = 0;
    virtual void    Close() = 0;
  };
}

#endif /* !IO_ACCEPTOR_H_ */
