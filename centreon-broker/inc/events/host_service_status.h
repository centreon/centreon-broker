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

#ifndef HOST_SERVICE_STATUS_H_
# define HOST_SERVICE_STATUS_H_

# include <string>
# include <sys/types.h>
# include "status.h"

namespace                     CentreonBroker
{
  namespace                   Events
  {
    /**
     *  This class represents what is shared between a host status event and a
     *  service status event.
     */
    class                     HostServiceStatus : public Status
    {
     private:
      enum                    Double
      {
	CHECK_INTERVAL = 0,
	EXECUTION_TIME,
	LATENCY,
	PERCENT_STATE_CHANGE,
	RETRY_INTERVAL,
	DOUBLE_NB
      };
      enum                    Int
      {
	MODIFIED_ATTRIBUTES = 0,
	INT_NB
      };
      enum                    Short
      {
	ACKNOWLEDGEMENT_TYPE = 0,
	ACTIVE_CHECKS_ENABLED,
	CHECK_TYPE,
	CURRENT_CHECK_ATTEMPT,
	CURRENT_NOTIFICATION_NUMBER,
	CURRENT_STATE,
	HAS_BEEN_CHECKED,
	IS_FLAPPING,
	LAST_HARD_STATE,
	MAX_CHECK_ATTEMPTS,
	NO_MORE_NOTIFICATIONS,
	OBSESS_OVER,
	PASSIVE_CHECKS_ENABLED,
	PROBLEM_HAS_BEEN_ACKNOWLEDGED,
	SCHEDULED_DOWNTIME_DEPTH,
	SHOULD_BE_SCHEDULED,
	STATE_TYPE,
	SHORT_NB
      };
      enum                    String
      {
	CHECK_COMMAND = 0,
	EVENT_HANDLER,
	HOST_NAME,
	OUTPUT,
	PERFDATA,
	STRING_NB
      };
      enum                    TimeT
      {
	LAST_CHECK = 0,
	LAST_HARD_STATE_CHANGE,
	LAST_NOTIFICATION,
	LAST_STATE_CHANGE,
	NEXT_CHECK,
	NEXT_NOTIFICATION,
	TIMET_NB
      };
      double             doubles_[DOUBLE_NB];
      int                ints_[INT_NB];
      short              shorts_[SHORT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      void               InternalCopy(const HostServiceStatus &hsse);

     public:
                         HostServiceStatus();
			 HostServiceStatus(const
			   HostServiceStatus& hsse);
			 ~HostServiceStatus();
      HostServiceStatus& operator=(const HostServiceStatus& hsse);
      // Getters
      short              GetAcknowledgementType() const throw ();
      short              GetActiveChecksEnabled() const throw ();
      const std::string& GetCheckCommand() const throw ();
      double             GetCheckInterval() const throw ();
      short              GetCheckType() const throw ();
      short              GetCurrentCheckAttempt() const throw ();
      short              GetCurrentNotificationNumber() const throw ();
      short              GetCurrentState() const throw ();
      const std::string& GetEventHandler() const throw ();
      double             GetExecutionTime() const throw ();
      short              GetHasBeenChecked() const throw ();
      const std::string& GetHostName() const throw ();
      short              GetIsFlapping() const throw ();
      time_t             GetLastCheck() const throw ();
      short              GetLastHardState() const throw ();
      time_t             GetLastHardStateChange() const throw ();
      time_t             GetLastNotification() const throw ();
      time_t             GetLastStateChange() const throw ();
      double             GetLatency() const throw ();
      short              GetMaxCheckAttempts() const throw ();
      int                GetModifiedAttributes() const throw ();
      time_t             GetNextCheck() const throw ();
      time_t             GetNextNotification() const throw ();
      short              GetNoMoreNotifications() const throw ();
      short              GetObsessOver() const throw ();
      const std::string& GetOutput() const throw ();
      short              GetPassiveChecksEnabled() const throw ();
      double             GetPercentStateChange() const throw ();
      const std::string& GetPerfdata() const throw ();
      short              GetProblemHasBeenAcknowledged() const throw ();
      double             GetRetryInterval() const throw ();
      short              GetScheduledDowntimeDepth() const throw ();
      short              GetShouldBeScheduled() const throw ();
      short              GetStateType() const throw ();
      // Setters
      void               SetAcknowledgementType(short at) throw ();
      void               SetActiveChecksEnabled(short ace) throw ();
      void               SetCheckCommand(const std::string& cc);
      void               SetCheckInterval(double ci) throw ();
      void               SetCheckType(short ct) throw ();
      void               SetCurrentCheckAttempt(short cca) throw ();
      void               SetCurrentNotificationNumber(short cnn) throw ();
      void               SetCurrentState(short cs) throw ();
      void               SetEventHandler(const std::string& eh);
      void               SetExecutionTime(double et) throw ();
      void               SetHasBeenChecked(short hbc) throw ();
      void               SetHostName(const std::string& hn);
      void               SetIsFlapping(short i_f) throw ();
      void               SetLastCheck(time_t lc) throw ();
      void               SetLastHardState(short lhs) throw ();
      void               SetLastHardStateChange(time_t lhsc) throw ();
      void               SetLastNotification(time_t ln) throw ();
      void               SetLastStateChange(time_t lsc) throw ();
      void               SetLatency(double l) throw ();
      void               SetMaxCheckAttempts(short mca) throw ();
      void               SetModifiedAttributes(int ma) throw ();
      void               SetNextCheck(time_t nc) throw ();
      void               SetNextNotification(time_t nn) throw ();
      void               SetNoMoreNotifications(short nmn) throw ();
      void               SetObsessOver(short oo) throw ();
      void               SetOutput(const std::string& o);
      void               SetPassiveChecksEnabled(short pce) throw ();
      void               SetPercentStateChange(double psc) throw ();
      void               SetPerfdata(const std::string& p);
      void               SetProblemHasBeenAcknowledged(short phba) throw ();
      void               SetRetryInterval(double ri) throw ();
      void               SetScheduledDowntimeDepth(short sdd) throw ();
      void               SetShouldBeScheduled(short sbs) throw ();
      void               SetStateType(short st) throw ();
    };
  }
}

#endif /* !HOST_SERVICE_STATUS_H_ */
