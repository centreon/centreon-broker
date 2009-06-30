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

#ifndef DOWNTIME_H_
# define DOWNTIME_H_

# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                Downtime : public Event
    {
     private:
      enum               Int
      {
	ACTUAL_END_TIME_USEC = 0,
	ACTUAL_START_TIME_USEC,
	INTERNAL_ID,
	TRIGGERED_BY_ID,
	INT_NB
      };
      enum               Short
      {
	DOWNTIME_TYPE = 0,
	DURATION,
	IS_FIXED,
	WAS_CANCELLED,
	WAS_STARTED,
	SHORT_NB
      };
      enum               String
      {
	AUTHOR_NAME = 0,
	COMMENT_DATA,
	HOST,
	SERVICE,
	STRING_NB
      };
      enum               TimeT
      {
	ACTUAL_END_TIME = 0,
	ACTUAL_START_TIME,
	ENTRY_TIME,
	SCHEDULED_END_TIME,
	SCHEDULED_START_TIME,
	TIMET_NB
      };
      int                ints_[INT_NB];
      short              shorts_[SHORT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      void               InternalCopy(const Downtime& downtime);

     public:
                         Downtime();
			 Downtime(const Downtime& downtime);
			 ~Downtime();
      Downtime&          operator=(const Downtime& downtime);
      time_t             GetActualEndTime() const throw ();
      int                GetActualEndTimeUsec() const throw ();
      time_t             GetActualStartTime() const throw ();
      int                GetActualStartTimeUsec() const throw ();
      const std::string& GetAuthorName() const throw ();
      const std::string& GetCommentData() const throw ();
      short              GetDowntimeType() const throw ();
      short              GetDuration() const throw ();
      time_t             GetEntryTime() const throw ();
      const std::string& GetHost() const throw ();
      int                GetInternalId() const throw ();
      short              GetIsFixed() const throw ();
      time_t             GetScheduledEndTime() const throw ();
      time_t             GetScheduledStartTime() const throw ();
      const std::string& GetService() const throw ();
      int                GetTriggeredById() const throw ();
      int                GetType() const throw ();
      short              GetWasCancelled() const throw ();
      short              GetWasStarted() const throw ();
      void               SetActualEndTime(time_t aet) throw ();
      void               SetActualEndTimeUsec(int aetu) throw ();
      void               SetActualStartTime(time_t ast) throw ();
      void               SetActualStartTimeUsec(int astu) throw ();
      void               SetAuthorName(const std::string& an);
      void               SetCommentData(const std::string& cd);
      void               SetDowntimeType(short dt) throw ();
      void               SetDuration(short d) throw ();
      void               SetEntryTime(time_t et) throw ();
      void               SetHost(const std::string& h);
      void               SetInternalId(int ii) throw ();
      void               SetIsFixed(int i_f) throw ();
      void               SetScheduledEndTime(time_t set) throw ();
      void               SetScheduledStartTime(time_t sst) throw ();
      void               SetService(const std::string& service);
      void               SetTriggeredById(int tbi) throw ();
      void               SetWasCancelled(short wc) throw ();
      void               SetWasStarted(short ws) throw ();
    };
  }
}

#endif /* !DOWNTIME_H_ */
