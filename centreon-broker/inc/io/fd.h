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

#ifndef IO_FD_H_
# define IO_FD_H_

# include "exception.h"
# include "io/io.h"

namespace           CentreonBroker
{
  namespace         IO
  {
    /**
     *  \class FDStream fd.h "io/fd.h"
     *  \brief File descriptor (FD) wrapper.
     *
     *  The FDStream class wraps read/write operations on the FD so that any FD
     *  can be used as an IO::Stream. This class only provides basic common
     *  operations on the file descriptor. If you want to have more specialized
     *  behavior, refer to more specialized classes of this header.
     *
     *  \see SocketStream
     */
    class           FDStream : public Stream
    {
     private:
      void          InternalClose() throw ();
      void          InternalCopy(const FDStream& fds)
                      throw (CentreonBroker::Exception);

     protected:
      int           fd_;

     public:
                    FDStream(int fd) throw ();
                    FDStream(const FDStream& fds)
                      throw (CentreonBroker::Exception);
      virtual       ~FDStream() throw ();
      FDStream&     operator=(const FDStream& fds)
                      throw (CentreonBroker::Exception);
      virtual void  Close() throw ();
      unsigned int  Receive(void* buffer, unsigned int size)
                      throw (CentreonBroker::Exception);
      unsigned int  Send(const void* buffer, unsigned int size)
                      throw (CentreonBroker::Exception);
    };

    /**
     *  \class SocketStream fd.h "io/fd.h"
     *  \brief Socket file descriptor wrapper.
     *
     *  Wraps a socket file descriptor. It subclasses FDStream but provides
     *  more socket-specific calls.
     *
     *  \see FDStream
     */
    class           SocketStream : public FDStream
    {
     public:
                    SocketStream(int sockfd) throw ();
                    SocketStream(const SocketStream& sockstream)
                      throw (CentreonBroker::Exception);
                    ~SocketStream() throw ();
      SocketStream& operator=(const SocketStream& sockstream)
                      throw (CentreonBroker::Exception);
      void          Close() throw ();
    };
  }
}

#endif /* !IO_FD_H_ */
