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

#ifndef PROTOCOL_SOCKET_H_
# define PROTOCOL_SOCKET_H_

# include <cstring>
# include <ctime>
# include "io/io.h"

namespace           CentreonBroker
{
  /**
   *  The ProtocolSocket class will buffer input from the socket and make it
   *  available a line at a time.
   */
  class             ProtocolSocket
  {
   private:
    char            buffer_[1024];
    unsigned long   bytes_processed_;
    size_t          discard_;
    time_t          last_checkin_time_;
    size_t          length_;
    unsigned long   lines_processed_;
    IO::Stream*     stream_;
                    ProtocolSocket(const ProtocolSocket& ps) throw ();
    ProtocolSocket& operator=(const ProtocolSocket& ps) throw ();

   public:
                    ProtocolSocket(IO::Stream* stream) throw ();
    virtual         ~ProtocolSocket();
    unsigned long   GetBytesProcessed() const;
    time_t          GetLastCheckinTime() const;
    char*           GetLine();
    unsigned long   GetLinesProcessed() const;
  };
}

#endif /* !PROTOCOL_SOCKET_H_ */
