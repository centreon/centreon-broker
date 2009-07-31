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

#ifndef IO_IO_H_
# define IO_IO_H_

namespace             CentreonBroker
{
  namespace           IO
  {
    /**
     *  \class Stream io.h "io/io.h"
     *  \brief This is the root of objects that support I/O operations.
     *
     *  A stream is an object on which it is possible to send data to and to
     *  receive data from. The obvious example of a concrete stream is a
     *  socket. The Stream interface does not define how the concrete object
     *  is build or connected or opened, but only that we can perform I/O
     *  operations on such objects.
     *
     *  \see Net4Stream
     *  \see Net6Stream
     *  \see UnixStream
     */
    class             Stream
    {
     protected:
                      Stream(const Stream& stream) throw ();
      Stream&         operator=(const Stream& stream) throw ();

     public:
                      Stream() throw ();
      virtual         ~Stream();
      virtual void    Close() = 0;
      virtual int     Receive(char* buffer, int size) = 0;
      virtual int     Send(const char* buffer, int size) = 0;
    };

    /**
     *  \class Acceptor io.h "io/io.h"
     *  \brief Accept incoming clients.
     *
     *  An Acceptor is a kind of 'gate' on which clients can come, eventually
     *  authentify, in order to generate a new session (represented by a Stream
     *  object) and perform I/O operations with the application. It is not
     *  defined within the Acceptor interface how the client connects to the
     *  acceptor or how or if it authenticates.
     *
     *  \see Net4Acceptor
     *  \see Net6Acceptor
     *  \see UnixAcceptor
     *  \see Stream
     */
    class             Acceptor
    {
     protected:
                      Acceptor(const Acceptor& acceptor) throw ();
      Acceptor&       operator=(const Acceptor& acceptor) throw ();

     public:
                      Acceptor() throw ();
      virtual         ~Acceptor();
      virtual Stream* Accept() = 0;
      virtual void    Close() = 0;
    };
  }
}

#endif /* !IO_IO_H_ */
