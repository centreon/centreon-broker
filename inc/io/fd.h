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

# include "io/stream.h"

namespace         IO
{
  /**
   *  \class FD fd.h "io/fd.h"
   *  \brief File descriptor (FD) wrapper.
   *
   *  The FD class wraps read/write operations on a file descriptor so that any
   *  FD can be used as an IO::Stream. This class only provides basic common
   *  operations on the file descriptor. If you want to have more specialized
   *  behavior, refer to more specialized classes.
   *
   *  \see IO::Net::Socket
   */
  class           FD : public Stream
  {
   private:
    void          InternalClose();
    void          InternalCopy(const FD& fd);

   protected:
    int           fd;

   public:
                  FD(int fildes = -1);
                  FD(const FD& fd);
    virtual       ~FD();
    FD&           operator=(const FD& fd);
    virtual void  Close();
    unsigned int  Receive(void* buffer, unsigned int size);
    unsigned int  Send(const void* buffer, unsigned int size);
  };
}

#endif /* !IO_FD_H_ */
