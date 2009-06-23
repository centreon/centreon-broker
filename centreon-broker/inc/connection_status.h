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

#ifndef CONNECTION_STATUS_H_
# define CONNECTION_STATUS_H_

# include <sys/types.h>
# include "event.h"

namespace             CentreonBroker
{
  class               ConnectionStatus : public Event
  {
   private:
    enum              Int
    {
      BYTES_PROCESSED = 0,
      ENTRIES_PROCESSED,
      LINES_PROCESSED,
      INT_NB
    };
    enum              TimeT
    {
      DATA_END_TIME = 0,
      DISCONNECT_TIME,
      LAST_CHECKIN_TIME,
      TIMET_NB
    };
    int               ints_[INT_NB];
    time_t            timets_[TIMET_NB];
    void              InternalCopy(const ConnectionStatus& cs) throw ();

   public:
                      ConnectionStatus() throw ();
                      ConnectionStatus(const ConnectionStatus& cs) throw ();
    virtual           ~ConnectionStatus() throw ();
    ConnectionStatus& operator=(const ConnectionStatus& cs) throw ();
    // Getters
    int               GetBytesProcessed() const throw ();
    time_t            GetDataEndTime() const throw ();
    time_t            GetDisconnectTime() const throw ();
    int               GetEntriesProcessed() const throw ();
    time_t            GetLastCheckinTime() const throw ();
    int               GetLinesProcessed() const throw ();
    virtual int       GetType() const throw ();
    // Setters
    void              SetBytesProcessed(int bp) throw ();
    void              SetDataEndTime(time_t det) throw ();
    void              SetDisconnectTime(time_t dt) throw ();
    void              SetEntriesProcessed(int ep) throw ();
    void              SetLastCheckinTime(time_t lct) throw ();
    void              SetLinesProcessed(int lp) throw ();
  };
}

#endif /* !CONNECTION_STATUS_H_ */
