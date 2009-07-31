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

#ifndef IO_NET4_H_
# define IO_NET4_H_

# include "exception.h"
# include "io/io.h"

namespace            CentreonBroker
{
  namespace          IO
  {
    /**
     *  \class Net4Stream net4.h "io/net4.h"
     *  \brief A connected IPv4 Berkeley style socket.
     *
     *  A Net4Stream is an already connected IPv4 socket. It is undefined how
     *  this socket has been initialized.
     *
     *  \see Net4Acceptor
     */
    class            Net4Stream : public Stream
    {
     private:
      int            sockfd_;
      void           InternalCopy(const Net4Stream& n4s)
                       throw (CentreonBroker::Exception);

     public:
                     Net4Stream(int sockfd) throw ();
                     Net4Stream(const Net4Stream& n4s)
                       throw (CentreonBroker::Exception);
                     ~Net4Stream() throw ();
      Net4Stream&    operator=(const Net4Stream& n4s)
                       throw (CentreonBroker::Exception);
      void           Close() throw ();
      int            Receive(char* buffer, int size)
                       throw (CentreonBroker::Exception);
      int            Send(const char* buffer, int size)
                       throw (CentreonBroker::Exception);
    };

    /**
     *  \class Net4Acceptor net4.h "io/net4.h"
     *  \brief Listen on a specified port to wait for incoming clients.
     *
     *  This class is used to listen on a specified port of the local host. If
     *  one network client connects to this port, the Net4Acceptor can generate
     *  a new Stream object corresponding to this specific client.
     *
     *  Usage is pretty simple. Just call the Listen() method with the desired
     *  port as argument. Then, just call Accept() to get the next available
     *  incoming client. Once you're over with the the acceptor, just call
     *  Close() to shut it down. If you want to, start the cycle again with a
     *  potentially different port.
     *
     *  \see Net4Stream
     */
    class            Net4Acceptor : public Acceptor
    {
     private:
      int            sockfd_;
      void           InternalCopy(const Net4Acceptor& n4a)
                       throw (CentreonBroker::Exception);

     public:
                     Net4Acceptor() throw ();
                     Net4Acceptor(const Net4Acceptor& n4a)
                       throw (CentreonBroker::Exception);
                     ~Net4Acceptor() throw ();
		     Net4Acceptor&  operator=(const Net4Acceptor& n4a)
                       throw (CentreonBroker::Exception);
      Stream*        Accept();
      void           Close() throw ();
      void           Listen(unsigned short port)
                       throw (CentreonBroker::Exception);
    };
  }
}

#endif /* !IO_NET4_H_ */
