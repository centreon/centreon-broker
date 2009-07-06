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

#ifndef PROGRAM_STATUS_H_
# define PROGRAM_STATUS_H_

# include <ctime>
# include <string>
# include "status.h"

namespace                CentreonBroker
{
  namespace              Events
  {
    class                ProgramStatus : public Status
    {
     private:
      enum               Bool
      {
	ACTIVE_HOST_CHECKS_ENABLED = 0,
	ACTIVE_SERVICE_CHECKS_ENABLED,
	DAEMON_MODE,
	IS_RUNNING,
	OBSESS_OVER_HOSTS,
	OBSESS_OVER_SERVICES,
	PASSIVE_HOST_CHECKS_ENABLED,
	PASSIVE_SERVICE_CHECKS_ENABLED,
	BOOL_NB
      };
      enum               Int
      {
	MODIFIED_HOST_ATTRIBUTES = 0,
	MODIFIED_SERVICE_ATTRIBUTES,
	PID,
	INT_NB
      };
      enum               String
      {
	GLOBAL_HOST_EVENT_HANDLER = 0,
	GLOBAL_SERVICE_EVENT_HANDLER,
	STRING_NB
      };
      enum               TimeT
      {
	LAST_ALIVE = 0,
	LAST_COMMAND_CHECK,
	LAST_LOG_ROTATION,
	PROGRAM_END_TIME,
	PROGRAM_START,
	TIMET_NB
      };
      bool               bools_[BOOL_NB];
      int                ints_[INT_NB];
      std::string        strings_[STRING_NB];
      time_t             timets_[TIMET_NB];
      void               InternalCopy(const ProgramStatus& ps);

     public:
                         ProgramStatus() throw ();
			 ProgramStatus(const ProgramStatus& ps);
			 ~ProgramStatus() throw ();
      ProgramStatus&     operator=(const ProgramStatus& ps);
      // Getters
      bool               GetActiveHostChecksEnabled() const throw ();
      bool               GetActiveServiceChecksEnabled() const throw ();
      bool               GetDaemonMode() const throw ();
      const std::string& GetGlobalHostEventHandler() const throw ();
      const std::string& GetGlobalServiceEventHandler() const throw ();
      bool               GetIsRunning() const throw ();
      time_t             GetLastAlive() const throw ();
      time_t             GetLastCommandCheck() const throw ();
      time_t             GetLastLogRotation() const throw ();
      int                GetModifiedHostAttributes() const throw ();
      int                GetModifiedServiceAttributes() const throw ();
      bool               GetObsessOverHosts() const throw ();
      bool               GetObsessOverServices() const throw ();
      bool               GetPassiveHostChecksEnabled() const throw ();
      bool               GetPassiveServiceChecksEnabled() const throw ();
      int                GetPid() const throw ();
      time_t             GetProgramEndTime() const throw ();
      time_t             GetProgramStart() const throw ();
      int                GetType() const throw ();
      // Setters
      void               SetActiveHostChecksEnabled(bool ahce) throw ();
      void               SetActiveServiceChecksEnabled(bool asce) throw ();
      void               SetDaemonMode(bool dm) throw ();
      void               SetGlobalHostEventHandler(const std::string& gheh);
      void               SetGlobalServiceEventHandler(const std::string& gseh);
      void               SetIsRunning(bool ir) throw ();
      void               SetLastAlive(time_t la) throw ();
      void               SetLastCommandCheck(time_t lcc) throw ();
      void               SetLastLogRotation(time_t llr) throw ();
      void               SetModifiedHostAttributes(int mha) throw ();
      void               SetModifiedServiceAttributes(int msa) throw ();
      void               SetObsessOverHosts(bool ooh) throw ();
      void               SetObsessOverServices(bool oos) throw ();
      void               SetPassiveHostChecksEnabled(bool phce) throw ();
      void               SetPassiveServiceChecksEnabled(bool psce) throw ();
      void               SetPid(int p) throw ();
      void               SetProgramEndTime(time_t pet) throw ();
      void               SetProgramStart(time_t ps) throw ();
    };
  }
}

#endif /* !PROGRAM_STATUS_H_ */
