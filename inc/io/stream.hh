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

#ifndef IO_STREAM_HH_
# define IO_STREAM_HH_

namespace                io {
  /**
   *  @class stream stream.hh "io/stream.hh"
   *  @brief This is the root of objects that support I/O operations.
   *
   *  A stream is an object on which it is possible to send data to and
   *  to receive data from. The obvious example of a concrete stream is
   *  a socket. The stream interface does not define how the concrete
   *  object is build or connected or opened, but only that we can
   *  perform I/O operations on such objects.
   *
   *  @see io::net::ipv4_stream
   *  @see io::net::ipv6_stream
   *  @see unix_stream
   */
  class                  stream {
   protected:
                         stream();
                         stream(stream const& s);
    stream&              operator=(stream const& s);

   public:
    virtual              ~stream();
    virtual void         close() = 0;
    virtual unsigned int receive(void* buffer, unsigned int size) = 0;
    virtual unsigned int send(void const* buffer, unsigned int size) = 0;
  };
}

#endif /* !IO_STREAM_HH_ */
