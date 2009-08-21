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

#ifndef IO_UNIX_H_
# define IO_UNIX_H_

# include "exception.h"
# include "io/io.h"

namespace            CentreonBroker
{
  namespace          IO
  {
    /**
     *  \class UnixAcceptor unix.h "io/unix.h"
     *  \brief Listen on a specified Unix domain socket for incoming clients.
     *
     *  This class is used to listen on a specified Unix domain socket on the
     *  local host. If one local client connects to this socket, the
     *  UnixAcceptor can generate a new Stream object corresponding to this
     *  specific client.
     *
     *  Usage is pretty simple. Just call the Listen() method with the desired
     *  socket name as argument. Then, just call Accept() to get the next
     *  available incoming client. Once you're over with the the acceptor, just
     *  call Close() to shut it down. If you want to, start the cycle again
     *  with a potentially different socket name.
     *
     *  \see SocketStream
     */
    class            UnixAcceptor : public Acceptor
    {
     private:
      int            sockfd_;
      void           InternalCopy(const UnixAcceptor& ua)
                       throw (CentreonBroker::Exception);

     public:
                     UnixAcceptor() throw ();
                     UnixAcceptor(const UnixAcceptor& ua)
                       throw (CentreonBroker::Exception);
                     ~UnixAcceptor() throw ();
      UnixAcceptor&  operator=(const UnixAcceptor& ua)
                       throw (CentreonBroker::Exception);
      Stream*        Accept();
      void           Close() throw ();
      void           Listen(const char* sock_path)
                       throw (CentreonBroker::Exception);
    };
  }
}

#endif /* !IO_UNIX_H_ */
