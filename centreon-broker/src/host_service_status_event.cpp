/*
** host_service_status_event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/15/09 Matthieu Kermagoret
** Last update 05/18/09 Matthieu Kermagoret
*/

#include <cstring>
#include <string>
#include "host_service_status_event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Make a copy of all internal members of HostServiceStatusEvent to the
 *  current instance.
 */
void HostServiceStatusEvent::InternalCopy(const HostServiceStatusEvent& hsse)
{
  memcpy(this->doubles_, hsse.doubles_, sizeof(this->doubles_));
  memcpy(this->ints_, hsse.ints_, sizeof(this->ints_));
  memcpy(this->shorts_, hsse.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = hsse.strings_[i];
  memcpy(this->timets_, hsse.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostServiceStatusEvent default constructor.
 */
HostServiceStatusEvent::HostServiceStatusEvent()
{
  for (unsigned int i = 0; i < DOUBLE_NB; i++)
    this->doubles_[i] = 0.0;
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  HostServiceStatusEvent copy constructor.
 */
HostServiceStatusEvent::HostServiceStatusEvent(const HostServiceStatusEvent& h)
  : StatusEvent(h)
{
  this->InternalCopy(h);
}

/**
 *  HostServiceStatusEvent destructor.
 */
HostServiceStatusEvent::~HostServiceStatusEvent()
{
}

/**
 *  HostServiceStatusEvent operator= overload.
 */
HostServiceStatusEvent& HostServiceStatusEvent::operator=(const
  HostServiceStatusEvent& hsse)
{
  this->StatusEvent::operator=(hsse);
  this->InternalCopy(hsse);
  return (*this);
}

/**
 *  Get the acknowledgement_type member.
 */
short HostServiceStatusEvent::GetAcknowledgementType() const throw ()
{
  return (this->shorts_[ACKNOWLEDGEMENT_TYPE]);
}

/**
 *  Get the active_checks_enabled member.
 */
short HostServiceStatusEvent::GetActiveChecksEnabled() const throw ()
{
  return (this->shorts_[ACTIVE_CHECKS_ENABLED]);
}

/**
 *  Get the check_command member.
 */
const std::string& HostServiceStatusEvent::GetCheckCommand() const throw ()
{
  return (this->strings_[CHECK_COMMAND]);
}

/**
 *  Get the check_timeperiod_object_id member.
 */
int HostServiceStatusEvent::GetCheckTimeperiodObjectId() const throw ()
{
  return (this->ints_[CHECK_TIMEPERIOD_OBJECT_ID]);
}

/**
 *  Get the check_type member.
 */
short HostServiceStatusEvent::GetCheckType() const throw ()
{
  return (this->shorts_[CHECK_TYPE]);
}

/**
 *  Get the current_check_attempt member.
 */
short HostServiceStatusEvent::GetCurrentCheckAttempt() const throw ()
{
  return (this->shorts_[CURRENT_CHECK_ATTEMPT]);
}

/**
 *  Get the current_notification_number member.
 */
short HostServiceStatusEvent::GetCurrentNotificationNumber() const throw ()
{
  return (this->shorts_[CURRENT_NOTIFICATION_NUMBER]);
}

/**
 *  Get the current_state member.
 */
short HostServiceStatusEvent::GetCurrentState() const throw ()
{
  return (this->shorts_[CURRENT_STATE]);
}

/**
 *  Get the event_handler member.
 */
const std::string& HostServiceStatusEvent::GetEventHandler() const throw ()
{
  return (this->strings_[EVENT_HANDLER]);
}

/**
 *  Get the execution_time member.
 */
double HostServiceStatusEvent::GetExecutionTime() const throw ()
{
  return (this->doubles_[EXECUTION_TIME]);
}

/**
 *  Get the has_been_checked member.
 */
short HostServiceStatusEvent::GetHasBeenChecked() const throw ()
{
  return (this->shorts_[HAS_BEEN_CHECKED]);
}

/**
 *  Get the is_flapping member.
 */
short HostServiceStatusEvent::GetIsFlapping() const throw ()
{
  return (this->shorts_[IS_FLAPPING]);
}

/**
 *  Get the last_check member.
 */
time_t HostServiceStatusEvent::GetLastCheck() const throw ()
{
  return (this->timets_[LAST_CHECK]);
}

/**
 *  Get the last_hard_state member.
 */
short HostServiceStatusEvent::GetLastHardState() const throw ()
{
  return (this->shorts_[LAST_HARD_STATE]);
}

/**
 *  Get the last_hard_state_change member.
 */
time_t HostServiceStatusEvent::GetLastHardStateChange() const throw ()
{
  return (this->timets_[LAST_HARD_STATE_CHANGE]);
}

/**
 *  Get the last_notification member.
 */
time_t HostServiceStatusEvent::GetLastNotification() const throw ()
{
  return (this->timets_[LAST_NOTIFICATION]);
}

/**
 *  Get the last_state_change member.
 */
time_t HostServiceStatusEvent::GetLastStateChange() const throw ()
{
  return (this->timets_[LAST_STATE_CHANGE]);
}

/**
 *  Get the latency member.
 */
double HostServiceStatusEvent::GetLatency() const throw ()
{
  return (this->doubles_[LATENCY]);
}

/**
 *  Get the max_check_attempts member.
 */
short HostServiceStatusEvent::GetMaxCheckAttempts() const throw ()
{
  return (this->shorts_[MAX_CHECK_ATTEMPTS]);
}

/**
 *  Get the modified_attributes member.
 */
int HostServiceStatusEvent::GetModifiedAttributes() const throw ()
{
  return (this->ints_[MODIFIED_ATTRIBUTES]);
}

/**
 *  Get the next_check member.
 */
time_t HostServiceStatusEvent::GetNextCheck() const throw ()
{
  return (this->timets_[NEXT_CHECK]);
}

/**
 *  Get the next_notification member.
 */
time_t HostServiceStatusEvent::GetNextNotification() const throw ()
{
  return (this->timets_[NEXT_NOTIFICATION]);
}

/**
 *  Get the no_more_notifications member.
 */
short HostServiceStatusEvent::GetNoMoreNotifications() const throw ()
{
  return (this->shorts_[NO_MORE_NOTIFICATIONS]);
}

/**
 *  Get the normal_check_interval member.
 */
double HostServiceStatusEvent::GetNormalCheckInterval() const throw ()
{
  return (this->doubles_[NORMAL_CHECK_INTERVAL]);
}

/**
 *  Get the obsess_over member.
 */
short HostServiceStatusEvent::GetObsessOver() const throw ()
{
  return (this->shorts_[OBSESS_OVER]);
}

/**
 *  Get the output member.
 */
const std::string& HostServiceStatusEvent::GetOutput() const throw ()
{
  return (this->strings_[OUTPUT]);
}

/**
 *  Get the passive_checks_enabled member.
 */
short HostServiceStatusEvent::GetPassiveChecksEnabled() const throw ()
{
  return (this->shorts_[PASSIVE_CHECKS_ENABLED]);
}

/**
 *  Get the percent_state_change member.
 */
double HostServiceStatusEvent::GetPercentStateChange() const throw ()
{
  return (this->doubles_[PERCENT_STATE_CHANGE]);
}

/**
 *  Get the perfdata member.
 */
const std::string& HostServiceStatusEvent::GetPerfdata() const throw ()
{
  return (this->strings_[PERFDATA]);
}

/**
 *  Get the problem_has_been_acknowledged member.
 */
short HostServiceStatusEvent::GetProblemHasBeenAcknowledged() const throw ()
{
  return (this->shorts_[PROBLEM_HAS_BEEN_ACKNOWLEDGED]);
}

/**
 *  Get the retry_check_interval member.
 */
double HostServiceStatusEvent::GetRetryCheckInterval() const throw ()
{
  return (this->doubles_[RETRY_CHECK_INTERVAL]);
}

/**
 *  Get the scheduled_downtime_depth member.
 */
short HostServiceStatusEvent::GetScheduledDowntimeDepth() const throw ()
{
  return (this->shorts_[SCHEDULED_DOWNTIME_DEPTH]);
}

/**
 *  Get the should_be_scheduled member.
 */
short HostServiceStatusEvent::GetShouldBeScheduled() const throw ()
{
  return (this->shorts_[SHOULD_BE_SCHEDULED]);
}

/**
 *  Get the state_type member.
 */
short HostServiceStatusEvent::GetStateType() const throw ()
{
  return (this->shorts_[STATE_TYPE]);
}

/**
 *  Set the acknowledgement_type member.
 */
void HostServiceStatusEvent::SetAcknowledgementType(short at) throw ()
{
  this->shorts_[ACKNOWLEDGEMENT_TYPE] = at;
  return ;
}

/**
 *  Set the active_checks_enabled member.
 */
void HostServiceStatusEvent::SetActiveChecksEnabled(short ace) throw ()
{
  this->shorts_[ACTIVE_CHECKS_ENABLED] = ace;
  return ;
}

/**
 *  Set the check_command member.
 */
void HostServiceStatusEvent::SetCheckCommand(const std::string& cc)
{
  this->strings_[CHECK_COMMAND] = cc;
  return ;
}

/**
 *  Set the check_timeperiod_object_id member.
 */
void HostServiceStatusEvent::SetCheckTimeperiodObjectId(int ctoi) throw ()
{
  this->ints_[CHECK_TIMEPERIOD_OBJECT_ID] = ctoi;
  return ;
}

/**
 *  Set the check_type member.
 */
void HostServiceStatusEvent::SetCheckType(short ct) throw ()
{
  this->shorts_[CHECK_TYPE] = ct;
  return ;
}

/**
 *  Set the current_check_attempt member.
 */
void HostServiceStatusEvent::SetCurrentCheckAttempt(short cca) throw ()
{
  this->shorts_[CURRENT_CHECK_ATTEMPT] = cca;
  return ;
}

/**
 *  Set the current_notification_number member.
 */
void HostServiceStatusEvent::SetCurrentNotificationNumber(short cnn) throw ()
{
  this->shorts_[CURRENT_NOTIFICATION_NUMBER] = cnn;
  return ;
}

/**
 *  Set the current_state member.
 */
void HostServiceStatusEvent::SetCurrentState(short cs) throw ()
{
  this->shorts_[CURRENT_STATE] = cs;
  return ;
}

/**
 *  Set the event_handler member.
 */
void HostServiceStatusEvent::SetEventHandler(const std::string& eh)
{
  this->strings_[EVENT_HANDLER] = eh;
  return ;
}

/**
 *  Set the execution_time member.
 */
void HostServiceStatusEvent::SetExecutionTime(double et) throw ()
{
  this->doubles_[EXECUTION_TIME] = et;
  return ;
}

/**
 *  Set the has_been_checked member.
 */
void HostServiceStatusEvent::SetHasBeenChecked(short hbc) throw ()
{
  this->shorts_[HAS_BEEN_CHECKED] = hbc;
  return ;
}

/**
 *  Set the is_flapping member.
 */
void HostServiceStatusEvent::SetIsFlapping(short i_f) throw ()
{
  this->shorts_[IS_FLAPPING] = i_f;
  return ;
}

/**
 *  Set the last_check member.
 */
void HostServiceStatusEvent::SetLastCheck(time_t lc) throw ()
{
  this->timets_[LAST_CHECK] = lc;
  return ;
}

/**
 *  Set the last_hard_state member.
 */
void HostServiceStatusEvent::SetLastHardState(short lhs) throw ()
{
  this->shorts_[LAST_HARD_STATE] = lhs;
  return ;
}

/**
 *  Set the last_hard_state_change member.
 */
void HostServiceStatusEvent::SetLastHardStateChange(time_t lhsc) throw ()
{
  this->timets_[LAST_HARD_STATE_CHANGE] = lhsc;
  return ;
}

/**
 *  Set the last_notification member.
 */
void HostServiceStatusEvent::SetLastNotification(time_t ln) throw ()
{
  this->timets_[LAST_NOTIFICATION] = ln;
  return ;
}

/**
 *  Set the last_state_change member.
 */
void HostServiceStatusEvent::SetLastStateChange(time_t lsc) throw ()
{
  this->timets_[LAST_STATE_CHANGE] = lsc;
  return ;
}

/**
 *  Set the latency member.
 */
void HostServiceStatusEvent::SetLatency(double l) throw ()
{
  this->doubles_[LATENCY] = l;
  return ;
}

/**
 *  Set the max_check_attempts member.
 */
void HostServiceStatusEvent::SetMaxCheckAttempts(short mca) throw ()
{
  this->shorts_[MAX_CHECK_ATTEMPTS] = mca;
  return ;
}

/**
 *  Set the modified_attributes member.
 */
void HostServiceStatusEvent::SetModifiedAttributes(int ma) throw ()
{
  this->ints_[MODIFIED_ATTRIBUTES] = ma;
  return ;
}

/**
 *  Set the next_check member.
 */
void HostServiceStatusEvent::SetNextCheck(time_t nc) throw ()
{
  this->timets_[NEXT_CHECK] = nc;
  return ;
}

/**
 *  Set the next_notification member.
 */
void HostServiceStatusEvent::SetNextNotification(time_t nn) throw ()
{
  this->timets_[NEXT_NOTIFICATION] = nn;
  return ;
}

/**
 *  Set the no_more_notifications member.
 */
void HostServiceStatusEvent::SetNoMoreNotifications(short nmn) throw ()
{
  this->shorts_[NO_MORE_NOTIFICATIONS] = nmn;
  return ;
}

/**
 *  Set the normal_check_interval member.
 */
void HostServiceStatusEvent::SetNormalCheckInterval(double nci) throw ()
{
  this->doubles_[NORMAL_CHECK_INTERVAL] = nci;
  return ;
}

/**
 *  Set the obsess_over member.
 */
void HostServiceStatusEvent::SetObsessOver(short oo) throw ()
{
  this->shorts_[OBSESS_OVER] = oo;
  return ;
}

/**
 *  Set the output member.
 */
void HostServiceStatusEvent::SetOutput(const std::string& o)
{
  this->strings_[OUTPUT] = o;
  return ;
}

/**
 *  Set the passive_checks_enabled member.
 */
void HostServiceStatusEvent::SetPassiveChecksEnabled(short pce) throw ()
{
  this->shorts_[PASSIVE_CHECKS_ENABLED] = pce;
  return ;
}

/**
 *  Set the percent_state_change member.
 */
void HostServiceStatusEvent::SetPercentStateChange(double psc) throw ()
{
  this->doubles_[PERCENT_STATE_CHANGE] = psc;
  return ;
}

/**
 *  Set the perfdata member.
 */
void HostServiceStatusEvent::SetPerfdata(const std::string& p)
{
  this->strings_[PERFDATA] = p;
  return ;
}

/**
 *  Set the problem_has_been_acknowledged member.
 */
void HostServiceStatusEvent::SetProblemHasBeenAcknowledged(short phba) throw ()
{
  this->shorts_[PROBLEM_HAS_BEEN_ACKNOWLEDGED] = phba;
  return ;
}

/**
 *  Set the retry_check_interval member.
 */
void HostServiceStatusEvent::SetRetryCheckInterval(double rci) throw ()
{
  this->doubles_[RETRY_CHECK_INTERVAL] = rci;
  return ;
}

/**
 *  Set the scheduled_downtime_depth member.
 */
void HostServiceStatusEvent::SetScheduledDowntimeDepth(short sdd) throw ()
{
  this->shorts_[SCHEDULED_DOWNTIME_DEPTH] = sdd;
  return ;
}

/**
 *  Set the should_be_scheduled member.
 */
void HostServiceStatusEvent::SetShouldBeScheduled(short sbs) throw ()
{
  this->shorts_[SHOULD_BE_SCHEDULED] = sbs;
  return ;
}

/**
 *  Set the state_type member.
 */
void HostServiceStatusEvent::SetStateType(short st) throw ()
{
  this->shorts_[STATE_TYPE] = st;
  return ;
}
