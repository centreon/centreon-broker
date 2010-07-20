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

#ifndef IO_STREAM_H_
# define IO_STREAM_H_

namespace                IO
{
  /**
   *  \class Stream stream.h "io/stream.h"
   *  \brief This is the root of objects that support I/O operations.
   *
   *  A stream is an object on which it is possible to send data to and to
   *  receive data from. The obvious example of a concrete stream is a socket.
   *  The Stream interface does not define how the concrete object is build or
   *  connected or opened, but only that we can perform I/O operations on such
   *  objects.
   *
   *  \see IO::Net::IPv4Stream
   *  \see IO::Net::IPv6Stream
   *  \see UnixStream
   */
  class                  Stream
  {
   protected:
                         Stream();
                         Stream(const Stream& stream);
    Stream&              operator=(const Stream& stream);

   public:
    virtual              ~Stream();
    virtual void         Close() = 0;
    virtual unsigned int Receive(void* buffer, unsigned int size) = 0;
    virtual unsigned int Send(const void* buffer, unsigned int size) = 0;
  };
}

#endif /* !IO_STREAM_H_ */
