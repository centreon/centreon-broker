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

# include <string>
# include <sys/types.h>
# include "status.h"

namespace              CentreonBroker
{
  class                ProgramStatus : public Status
  {
   private:
    enum               Int
    {
      MODIFIED_HOST_ATTRIBUTES = 0,
      MODIFIED_SERVICE_ATTRIBUTES,
      PROCESS_ID,
      INT_NB
    };
    enum               Short
    {
      ACTIVE_HOST_CHECKS_ENABLED = 0,
      ACTIVE_SERVICE_CHECKS_ENABLED,
      DAEMON_MODE,
      IS_CURRENTLY_RUNNING,
      OBSESS_OVER_HOSTS,
      OBSESS_OVER_SERVICES,
      PASSIVE_HOST_CHECKS_ENABLED,
      PASSIVE_SERVICE_CHECKS_ENABLED,
      SHORT_NB
    };
    enum               String
    {
      GLOBAL_HOST_EVENT_HANDLER = 0,
      GLOBAL_SERVICE_EVENT_HANDLER,
      STRING_NB
    };
    enum               TimeT
    {
      LAST_COMMAND_CHECK = 0,
      LAST_LOG_ROTATION,
      PROGRAM_END_TIME,
      PROGRAM_START_TIME,
      TIMET_NB
    };
    int                ints_[INT_NB];
    short              shorts_[SHORT_NB];
    std::string        strings_[STRING_NB];
    time_t             timets_[TIMET_NB];
    void               InternalCopy(const ProgramStatus& ps);

   public:
                       ProgramStatus() throw ();
                       ProgramStatus(const ProgramStatus& ps);
                       ~ProgramStatus() throw ();
    ProgramStatus&     operator=(const ProgramStatus& ps);
    // Getters
    short              GetActiveHostChecksEnabled() const throw ();
    short              GetActiveServiceChecksEnabled() const throw ();
    short              GetDaemonMode() const throw ();
    const std::string& GetGlobalHostEventHandler() const throw ();
    const std::string& GetGlobalServiceEventHandler() const throw ();
    short              GetIsCurrentlyRunning() const throw ();
    time_t             GetLastCommandCheck() const throw ();
    time_t             GetLastLogRotation() const throw ();
    int                GetModifiedHostAttributes() const throw ();
    int                GetModifiedServiceAttributes() const throw ();
    short              GetObsessOverHosts() const throw ();
    short              GetObsessOverServices() const throw ();
    short              GetPassiveHostChecksEnabled() const throw ();
    short              GetPassiveServiceChecksEnabled() const throw ();
    int                GetProcessId() const throw ();
    time_t             GetProgramEndTime() const throw ();
    time_t             GetProgramStartTime() const throw ();
    int                GetType() const throw ();
    // Setters
    void               SetActiveHostChecksEnabled(short ahce) throw ();
    void               SetActiveServiceChecksEnabled(short asce) throw ();
    void               SetDaemonMode(short dm) throw ();
    void               SetGlobalHostEventHandler(const std::string& gheh);
    void               SetGlobalServiceEventHandler(const std::string& gseh);
    void               SetIsCurrentlyRunning(short icr) throw ();
    void               SetLastCommandCheck(time_t lcc) throw ();
    void               SetLastLogRotation(time_t llr) throw ();
    void               SetModifiedHostAttributes(int mha) throw ();
    void               SetModifiedServiceAttributes(int msa) throw ();
    void               SetObsessOverHosts(short ooh) throw ();
    void               SetObsessOverServices(short oos) throw ();
    void               SetPassiveHostChecksEnabled(short phce) throw ();
    void               SetPassiveServiceChecksEnabled(short psce) throw ();
    void               SetProcessId(int pi) throw ();
    void               SetProgramEndTime(time_t pet) throw ();
    void               SetProgramStartTime(time_t pst) throw ();
  };
}

#endif /* !PROGRAM_STATUS_H_ */
