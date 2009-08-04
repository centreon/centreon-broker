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

#ifndef EVENTS_CONNECTION_STATUS_H_
# define EVENTS_CONNECTION_STATUS_H_

# include <ctime>
# include "events/event.h"

namespace               CentreonBroker
{
  namespace             Events
  {
    class               ConnectionStatus : public Event
    {
     private:
      void              InternalCopy(const ConnectionStatus& cs) throw ();

     public:
      int               bytes_processed;
      time_t            data_end_time;
      time_t            disconnect_time;
      int               entries_processed;
      time_t            last_checkin_time;
      int               lines_processed;
                        ConnectionStatus();
			ConnectionStatus(const ConnectionStatus& cs);
      virtual           ~ConnectionStatus();
      ConnectionStatus& operator=(const ConnectionStatus& cs);
      virtual int       GetType() const throw ();
    };
  }
}

#endif /* !EVENTS_CONNECTION_STATUS_H_ */
