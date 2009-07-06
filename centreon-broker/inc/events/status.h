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

#ifndef STATUS_H_
# define STATUS_H_

# include <ctime>
# include "events/event.h"

namespace         CentreonBroker
{
  namespace       Events
  {
    /**
     *  This is the root class of status events : host, program and service
     *  status events.
     */
    class         Status : public Event
    {
     private:
      enum        Bool
      {
	EVENT_HANDLER_ENABLED = 0,
	FAILURE_PREDICTION_ENABLED,
	FLAP_DETECTION_ENABLED,
	NOTIFICATIONS_ENABLED,
	PROCESS_PERFORMANCE_DATA,
	BOOL_NB
      };
      bool        bools_[BOOL_NB];
      void        InternalCopy(const Status& se) throw ();

     public:
                  Status();
		  Status(const Status& se);
      virtual     ~Status();
      Status&     operator=(const Status& se);
      // Getters
      bool        GetEventHandlerEnabled() const throw ();
      bool        GetFailurePredictionEnabled() const throw ();
      bool        GetFlapDetectionEnabled() const throw ();
      bool        GetNotificationsEnabled() const throw ();
      bool        GetProcessPerformanceData() const throw ();
      virtual int GetType() const throw () = 0;
      // Setters
      void        SetEventHandlerEnabled(bool ehe) throw ();
      void        SetFailurePredictionEnabled(bool fpe) throw ();
      void        SetFlapDetectionEnabled(bool fde) throw ();
      void        SetNotificationsEnabled(bool ne) throw ();
      void        SetProcessPerformanceData(bool ppd) throw ();
    };
  }
}

#endif /* !STATUS_H_ */
