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

#ifndef EVENTS_LOG_H_
# define EVENTS_LOG_H_

# include <ctime>
# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                Log : public Event
    {
     private:
      enum               Int
      {
	MSG_TYPE = 0,
	RETRY,
	INT_NB
      };
      enum               String
      {
	HOST_NAME = 0,
	LOG_TYPE,
	NOTIFICATION_CMD,
	NOTIFICATION_CONTACT,
	OUTPUT,
	SERVICE_DESCRIPTION,
	STATUS,
	STRING_NB
      };
      enum               TimeT
      {
	C_TIME = 0,
	TIMET_NB
      };
      int                ints_[INT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      void               InternalCopy(const Log& log);

     public:
                         Log();
                         Log(const Log& log);
                         ~Log();
      Log&               operator=(const Log& log);
      time_t             GetCtime() const throw ();
      const std::string& GetHostName() const throw ();
      const std::string& GetLogType() const throw ();
      int                GetMsgType() const throw ();
      const std::string& GetNotificationCmd() const throw ();
      const std::string& GetNotificationContact() const throw ();
      const std::string& GetOutput() const throw ();
      int                GetRetry() const throw ();
      const std::string& GetServiceDescription() const throw ();
      const std::string& GetStatus() const throw ();
      int                GetType() const throw ();
      void               SetCtime(time_t c) throw ();
      void               SetHostName(const std::string& hn);
      void               SetLogType(const std::string& lt);
      void               SetMsgType(int mt) throw ();
      void               SetNotificationCmd(const std::string& nc);
      void               SetNotificationContact(const std::string& nc);
      void               SetOutput(const std::string& o);
      void               SetRetry(int r) throw ();
      void               SetServiceDescription(const std::string& sd);
      void               SetStatus(const std::string& s);
    };
  }
}

#endif /* !EVENTS_LOG_H_ */
