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

# include <ctime>
# include <string>
# include "events/event.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    /**
     *  \class Downtime downtime.h "events/downtime.h"
     *  \brief Represents a downtime inside Nagios.
     *
     *  A Nagios user may have the ability to define downtimes, which are time
     *  periods inside which some host or service shall not generate any
     *  notification. This can occur when an admin sys perform maintenance on a
     *  server for example.
     */
    class                Downtime : public Event
    {
     private:
      enum               Int
      {
	INTERNAL_ID = 0,
	TRIGGERED_BY,
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
	END_TIME,
	ENTRY_TIME,
	START_TIME,
	TIMET_NB
      };
      int                ints_[INT_NB];
      short              shorts_[SHORT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      /**
       *  \brief Copy all internal data of the given object to the current
       *         instance.
       */
      void               InternalCopy(const Downtime& downtime);

     public:
      /**
       *  \brief Downtime default constructor.
       */
                         Downtime();
      /**
       *  \brief Downtime copy constructor.
       */
			 Downtime(const Downtime& downtime);
      /**
       *  \brief Downtime destructor.
       */
			 ~Downtime();
      /**
       *  \brief Overload of the = operator.
       */
      Downtime&          operator=(const Downtime& downtime);
      /**
       *  \brief XXX : need fix
       */
      time_t             GetActualEndTime() const throw ();
      /**
       *  \brief XXX : need fix
       */
      time_t             GetActualStartTime() const throw ();
      /**
       *  \brief Get the name of the user who defined the downtime.
       */
      const std::string& GetAuthorName() const throw ();
      /**
       *  \brief Get the comment associated with the downtime.
       */
      const std::string& GetCommentData() const throw ();
      /**
       *  \brief Get the type of the downtime.
       */
      short              GetDowntimeType() const throw ();
      /**
       *  \brief Get the duration of the downtime.
       */
      short              GetDuration() const throw ();
      /**
       *  \brief XXX : need fix
       */
      time_t             GetEndTime() const throw ();
      /**
       *  \brief XXX : need fix
       */
      time_t             GetEntryTime() const throw ();
      /**
       *  \brief Get the name of the host associated with the downtime.
       */
      const std::string& GetHost() const throw ();
      /**
       *  \brief XXX : need fix
       */
      int                GetInternalId() const throw ();
      /**
       *  \brief Determines whether or not the downtime is fixed.
       */
      short              GetIsFixed() const throw ();
      /**
       *  \brief XXX : need fix
       */
      time_t             GetStartTime() const throw ();
      /**
       *  \brief Get the name of the service associated with the downtime.
       */
      const std::string& GetService() const throw ();
      /**
       *  \brief XXX : need fix
       */
      int                GetTriggeredBy() const throw ();
      /**
       *  \brief Returns the type of this event (Event::DOWNTIME).
       */
      int                GetType() const throw ();
      /**
       *  \brief Determines whether or not the downtime was cancelled.
       */
      short              GetWasCancelled() const throw ();
      /**
       *  \brief Determines whether or not the downtime was started.
       */
      short              GetWasStarted() const throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetActualEndTime(time_t aet) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetActualStartTime(time_t ast) throw ();
      /**
       *  \brief Set the name of the user who defined the downtime.
       */
      void               SetAuthorName(const std::string& an);
      /**
       *  \brief Set the comment associated with the downtime.
       */
      void               SetCommentData(const std::string& cd);
      /**
       *  \brief Set the type of the downtime.
       */
      void               SetDowntimeType(short dt) throw ();
      /**
       *  \brief Set the duration of the downtime.
       */
      void               SetDuration(short d) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetEndTime(time_t et) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetEntryTime(time_t et) throw ();
      /**
       *  \brief Set the name of the host associated with the downtime.
       */
      void               SetHost(const std::string& h);
      /**
       *  \brief XXX : need fix
       */
      void               SetInternalId(int ii) throw ();
      /**
       *  \brief Set whether or not the downtime is fixed.
       */
      void               SetIsFixed(int i_f) throw ();
      /**
       *  \brief XXX : need fix
       */
      void               SetStartTime(time_t st) throw ();
      /**
       *  \brief Set the name of the service associated with the downtime.
       */
      void               SetService(const std::string& service);
      /**
       *  \brief XXX : need fix
       */
      void               SetTriggeredBy(int tb) throw ();
      /**
       *  \brief Set whether or not the downtime was cancelled.
       */
      void               SetWasCancelled(short wc) throw ();
      /**
       *  \brief Set whether or not the downtime was started.
       */
      void               SetWasStarted(short ws) throw ();
    };
  }
}

#endif /* !DOWNTIME_H_ */
