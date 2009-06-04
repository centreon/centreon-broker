/*
** program_status.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/22/09 Matthieu Kermagoret
** Last update 06/04/09 Matthieu Kermagoret
*/

#include "program_status.h"

using namespace CentreonBroker;

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
  memcpy(this->ints_, ps.ints_, sizeof(this->ints_));
  memcpy(this->shorts_, ps.shorts_, sizeof(this->shorts_));
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
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
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
short ProgramStatus::GetActiveHostChecksEnabled() const throw ()
{
  return (this->shorts_[ACTIVE_HOST_CHECKS_ENABLED]);
}

/**
 *  Get the active_service_checks_enabled member.
 */
short ProgramStatus::GetActiveServiceChecksEnabled() const throw ()
{
  return (this->shorts_[ACTIVE_SERVICE_CHECKS_ENABLED]);
}

/**
 *  Get the daemon_mode member.
 */
short ProgramStatus::GetDaemonMode() const throw ()
{
  return (this->shorts_[DAEMON_MODE]);
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
 *  Get the is_currently_running member.
 */
short ProgramStatus::GetIsCurrentlyRunning() const throw ()
{
  return (this->shorts_[IS_CURRENTLY_RUNNING]);
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
short ProgramStatus::GetObsessOverHosts() const throw ()
{
  return (this->shorts_[OBSESS_OVER_HOSTS]);
}

/**
 *  Get the obsess_over_services member.
 */
short ProgramStatus::GetObsessOverServices() const throw ()
{
  return (this->shorts_[OBSESS_OVER_SERVICES]);
}

/**
 *  Get the passive_host_checks_enabled member.
 */
short ProgramStatus::GetPassiveHostChecksEnabled() const throw ()
{
  return (this->shorts_[PASSIVE_HOST_CHECKS_ENABLED]);
}

/**
 *  Get the passive_service_checks_enabled member.
 */
short ProgramStatus::GetPassiveServiceChecksEnabled() const throw ()
{
  return (this->shorts_[PASSIVE_SERVICE_CHECKS_ENABLED]);
}

/**
 *  Get the process_id member.
 */
int ProgramStatus::GetProcessId() const throw ()
{
  return (this->ints_[PROCESS_ID]);
}

/**
 *  Get the program_end_time member.
 */
time_t ProgramStatus::GetProgramEndTime() const throw ()
{
  return (this->timets_[PROGRAM_END_TIME]);
}

/**
 *  Get the program_start_time member.
 */
time_t ProgramStatus::GetProgramStartTime() const throw ()
{
  return (this->timets_[PROGRAM_START_TIME]);
}

/**
 *  Get the type of the event.
 */
int ProgramStatus::GetType() const throw ()
{
  // XXX : hardcoded value
  return (6);
}

/**
 *  Set the active_host_checks_enabled member.
 */
void ProgramStatus::SetActiveHostChecksEnabled(short ahce) throw ()
{
  this->shorts_[ACTIVE_HOST_CHECKS_ENABLED] = ahce;
  return ;
}

/**
 *  Set the active_service_checks_enabled member.
 */
void ProgramStatus::SetActiveServiceChecksEnabled(short asce) throw ()
{
  this->shorts_[ACTIVE_SERVICE_CHECKS_ENABLED] = asce;
  return ;
}

/**
 *  Set the daemon_mode member.
 */
void ProgramStatus::SetDaemonMode(short dm) throw ()
{
  this->shorts_[DAEMON_MODE] = dm;
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
 *  Set the is_currently running member.
 */
void ProgramStatus::SetIsCurrentlyRunning(short icr) throw ()
{
  this->shorts_[IS_CURRENTLY_RUNNING] = icr;
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
void ProgramStatus::SetObsessOverHosts(short ooh) throw ()
{
  this->shorts_[OBSESS_OVER_HOSTS] = ooh;
  return ;
}

/**
 *  Set the obsess_over_services member.
 */
void ProgramStatus::SetObsessOverServices(short oos) throw ()
{
  this->shorts_[OBSESS_OVER_SERVICES] = oos;
  return ;
}

/**
 *  Set the passive_host_checks_enabled member.
 */
void ProgramStatus::SetPassiveHostChecksEnabled(short phce) throw ()
{
  this->shorts_[PASSIVE_HOST_CHECKS_ENABLED] = phce;
  return ;
}

/**
 *  Set the passive_service_checks_enabled member.
 */
void ProgramStatus::SetPassiveServiceChecksEnabled(short psce) throw ()
{
  this->shorts_[PASSIVE_SERVICE_CHECKS_ENABLED] = psce;
  return ;
}

/**
 *  Set the process_id member.
 */
void ProgramStatus::SetProcessId(int pi) throw ()
{
  this->ints_[PROCESS_ID] = pi;
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
 *  Set the program_start_time member.
 */
void ProgramStatus::SetProgramStartTime(time_t pst) throw ()
{
  this->timets_[PROGRAM_START_TIME] = pst;
  return ;
}
