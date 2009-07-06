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

#include "events/program_status.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the ProgramStatus object to the current instance.
 */
void ProgramStatus::InternalCopy(const ProgramStatus& ps)
{
  memcpy(this->bools_, ps.bools_, sizeof(this->bools_));
  memcpy(this->ints_, ps.ints_, sizeof(this->ints_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = ps.strings_[i];
  memcpy(this->timets_, ps.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  ProgramStatus default constructor.
 */
ProgramStatus::ProgramStatus() throw ()
{
  memset(this->bools_, 0, sizeof(this->bools_));
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  ProgramStatus copy constructor.
 */
ProgramStatus::ProgramStatus(const ProgramStatus& ps) : Status(ps)
{
  this->InternalCopy(ps);
}

/**
 *  ProgramStatus destructor.
 */
ProgramStatus::~ProgramStatus() throw ()
{
}

/**
 *  ProgramStatus operator= overload.
 */
ProgramStatus& ProgramStatus::operator=(const ProgramStatus& ps)
{
  this->Status::operator=(ps);
  this->InternalCopy(ps);
  return (*this);
}

/**
 *  Get the active_host_checks_enabled member.
 */
bool ProgramStatus::GetActiveHostChecksEnabled() const throw ()
{
  return (this->bools_[ACTIVE_HOST_CHECKS_ENABLED]);
}

/**
 *  Get the active_service_checks_enabled member.
 */
bool ProgramStatus::GetActiveServiceChecksEnabled() const throw ()
{
  return (this->bools_[ACTIVE_SERVICE_CHECKS_ENABLED]);
}

/**
 *  Get the daemon_mode member.
 */
bool ProgramStatus::GetDaemonMode() const throw ()
{
  return (this->bools_[DAEMON_MODE]);
}

/**
 *  Get the global_host_event_handler member.
 */
const std::string& ProgramStatus::GetGlobalHostEventHandler() const throw ()
{
  return (this->strings_[GLOBAL_HOST_EVENT_HANDLER]);
}

/**
 *  Get the gobal_service_event_handler member.
 */
const std::string& ProgramStatus::GetGlobalServiceEventHandler() const throw ()
{
  return (this->strings_[GLOBAL_SERVICE_EVENT_HANDLER]);
}

/**
 *  Get the is_running member.
 */
bool ProgramStatus::GetIsRunning() const throw ()
{
  return (this->bools_[IS_RUNNING]);
}

/**
 *  Get the last_alive member.
 */
time_t ProgramStatus::GetLastAlive() const throw ()
{
  return (this->timets_[LAST_ALIVE]);
}

/**
 *  Get the last_command_check member.
 */
time_t ProgramStatus::GetLastCommandCheck() const throw ()
{
  return (this->timets_[LAST_COMMAND_CHECK]);
}

/**
 *  Get the last_log_rotation member.
 */
time_t ProgramStatus::GetLastLogRotation() const throw ()
{
  return (this->timets_[LAST_LOG_ROTATION]);
}

/**
 *  Get the modified_host_attributes member.
 */
int ProgramStatus::GetModifiedHostAttributes() const throw ()
{
  return (this->ints_[MODIFIED_HOST_ATTRIBUTES]);
}

/**
 *  Get the modified_service_attributes member.
 */
int ProgramStatus::GetModifiedServiceAttributes() const throw ()
{
  return (this->ints_[MODIFIED_SERVICE_ATTRIBUTES]);
}

/**
 *  Get the obsess_over_hosts member.
 */
bool ProgramStatus::GetObsessOverHosts() const throw ()
{
  return (this->bools_[OBSESS_OVER_HOSTS]);
}

/**
 *  Get the obsess_over_services member.
 */
bool ProgramStatus::GetObsessOverServices() const throw ()
{
  return (this->bools_[OBSESS_OVER_SERVICES]);
}

/**
 *  Get the passive_host_checks_enabled member.
 */
bool ProgramStatus::GetPassiveHostChecksEnabled() const throw ()
{
  return (this->bools_[PASSIVE_HOST_CHECKS_ENABLED]);
}

/**
 *  Get the passive_service_checks_enabled member.
 */
bool ProgramStatus::GetPassiveServiceChecksEnabled() const throw ()
{
  return (this->bools_[PASSIVE_SERVICE_CHECKS_ENABLED]);
}

/**
 *  Get the pid member.
 */
int ProgramStatus::GetPid() const throw ()
{
  return (this->ints_[PID]);
}

/**
 *  Get the program_end_time member.
 */
time_t ProgramStatus::GetProgramEndTime() const throw ()
{
  return (this->timets_[PROGRAM_END_TIME]);
}

/**
 *  Get the program_start member.
 */
time_t ProgramStatus::GetProgramStart() const throw ()
{
  return (this->timets_[PROGRAM_START]);
}

/**
 *  Get the type of the event.
 */
int ProgramStatus::GetType() const throw ()
{
  return (Event::PROGRAMSTATUS);
}

/**
 *  Set the active_host_checks_enabled member.
 */
void ProgramStatus::SetActiveHostChecksEnabled(bool ahce) throw ()
{
  this->bools_[ACTIVE_HOST_CHECKS_ENABLED] = ahce;
  return ;
}

/**
 *  Set the active_service_checks_enabled member.
 */
void ProgramStatus::SetActiveServiceChecksEnabled(bool asce) throw ()
{
  this->bools_[ACTIVE_SERVICE_CHECKS_ENABLED] = asce;
  return ;
}

/**
 *  Set the daemon_mode member.
 */
void ProgramStatus::SetDaemonMode(bool dm) throw ()
{
  this->bools_[DAEMON_MODE] = dm;
  return ;
}

/**
 *  Set the global_host_event_handler member.
 */
void ProgramStatus::SetGlobalHostEventHandler(const std::string& gheh)
{
  this->strings_[GLOBAL_HOST_EVENT_HANDLER] = gheh;
  return ;
}

/**
 *  Set the global_service_event_handler member.
 */
void ProgramStatus::SetGlobalServiceEventHandler(const std::string& gseh)
{
  this->strings_[GLOBAL_SERVICE_EVENT_HANDLER] = gseh;
  return ;
}

/**
 *  Set the is_running member.
 */
void ProgramStatus::SetIsRunning(bool ir) throw ()
{
  this->bools_[IS_RUNNING] = ir;
  return ;
}

/**
 *  Set the last_alive member.
 */
void ProgramStatus::SetLastAlive(time_t la) throw ()
{
  this->timets_[LAST_ALIVE] = la;
  return ;
}

/**
 *  Set the last_command_check member.
 */
void ProgramStatus::SetLastCommandCheck(time_t lcc) throw ()
{
  this->timets_[LAST_COMMAND_CHECK] = lcc;
  return ;
}

/**
 *  Set the last_log_rotation member.
 */
void ProgramStatus::SetLastLogRotation(time_t llr) throw ()
{
  this->timets_[LAST_LOG_ROTATION] = llr;
  return ;
}

/**
 *  Set the modified_host_attributes member.
 */
void ProgramStatus::SetModifiedHostAttributes(int mha) throw ()
{
  this->ints_[MODIFIED_HOST_ATTRIBUTES] = mha;
  return ;
}

/**
 *  Set the modified_service_attributes member.
 */
void ProgramStatus::SetModifiedServiceAttributes(int msa) throw ()
{
  this->ints_[MODIFIED_SERVICE_ATTRIBUTES] = msa;
  return ;
}

/**
 *  Set the obsess_over_hosts member.
 */
void ProgramStatus::SetObsessOverHosts(bool ooh) throw ()
{
  this->bools_[OBSESS_OVER_HOSTS] = ooh;
  return ;
}

/**
 *  Set the obsess_over_services member.
 */
void ProgramStatus::SetObsessOverServices(bool oos) throw ()
{
  this->bools_[OBSESS_OVER_SERVICES] = oos;
  return ;
}

/**
 *  Set the passive_host_checks_enabled member.
 */
void ProgramStatus::SetPassiveHostChecksEnabled(bool phce) throw ()
{
  this->bools_[PASSIVE_HOST_CHECKS_ENABLED] = phce;
  return ;
}

/**
 *  Set the passive_service_checks_enabled member.
 */
void ProgramStatus::SetPassiveServiceChecksEnabled(bool psce) throw ()
{
  this->bools_[PASSIVE_SERVICE_CHECKS_ENABLED] = psce;
  return ;
}

/**
 *  Set the pid member.
 */
void ProgramStatus::SetPid(int p) throw ()
{
  this->ints_[PID] = p;
  return ;
}

/**
 *  Set the program_end_time member.
 */
void ProgramStatus::SetProgramEndTime(time_t pet) throw ()
{
  this->timets_[PROGRAM_END_TIME] = pet;
  return ;
}

/**
 *  Set the program_start member.
 */
void ProgramStatus::SetProgramStart(time_t ps) throw ()
{
  this->timets_[PROGRAM_START] = ps;
  return ;
}
