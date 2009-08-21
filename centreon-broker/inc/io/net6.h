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

#ifndef IO_NET6_H_
# define IO_NET6_H_

# include "exception.h"
# include "io/io.h"

namespace            CentreonBroker
{
  namespace          IO
  {
    /**
     *  \class Net6Acceptor net6.h "io/net6.h"
     *  \brief Listen on a specified port to wait for incoming clients.
     *
     *  This class is used to listen on a specified port of the local host. If
     *  one network client connects to this port, the Net6Acceptor can generate
     *  a new Stream object corresponding to this specific client.
     *
     *  Usage is pretty simple. Just call the Listen() method with the desired
     *  port as argument. Then, just call Accept() to get the next available
     *  incoming client. Once you're over with the the acceptor, just call
     *  Close() to shut it down. If you want to, start the cycle again with a
     *  potentially different port.
     *
     *  \see SocketStream
     */
    class            Net6Acceptor : public Acceptor
    {
     private:
      int            sockfd_;
      void           InternalCopy(const Net6Acceptor& n6a)
                       throw (CentreonBroker::Exception);

     public:
                     Net6Acceptor() throw ();
                     Net6Acceptor(const Net6Acceptor& n6a)
                       throw (CentreonBroker::Exception);
                     ~Net6Acceptor() throw ();
      Net6Acceptor&  operator=(const Net6Acceptor& n6a)
                       throw (CentreonBroker::Exception);
      Stream*        Accept();
      void           Close() throw ();
      void           Listen(unsigned short port, const char* iface = NULL)
                       throw (CentreonBroker::Exception);
    };
  }
}

#endif /* !IO_NET6_H_ */
