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

#ifndef IO_FD_HH_
# define IO_FD_HH_

# include "io/stream.hh"

namespace         io {
  /**
   *  @class fd fd.hh "io/fd.hh"
   *  @brief File descriptor (FD) wrapper.
   *
   *  The FD class wraps read/write operations on a file descriptor so
   *  that any FD can be used as an io::stream. This class only provides
   *  basic common operations on the file descriptor. If you want to
   *  have more specialized behavior, refer to more specialized classes.
   *
   *  @see io::net::socket
   */
  class           fd : public stream {
   private:
    void          _internal_close();
    void          _internal_copy(fd const& f);

   protected:
    int           _fd;

   public:
                  fd(int fildes = -1);
                  fd(fd const& f);
    virtual       ~fd();
    fd&           operator=(fd const& f);
    virtual void  close();
    unsigned int  receive(void* buffer, unsigned int size);
    unsigned int  send(const void* buffer, unsigned int size);
  };
}

#endif /* !IO_FD_HH_ */
