/*
** service_status_event.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/15/09 Matthieu Kermagoret
*/

#include "eventsubscriber.h"
#include "service_status_event.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  ServiceStatusEvent constructor.
 */
ServiceStatusEvent::ServiceStatusEvent()
{
  this->status_update_time = 0;
  this->current_state = 0;
  this->has_been_checked = 0;
  this->should_be_scheduled = 0;
  this->current_check_attempt = 0;
  this->max_check_attempts = 0;
  this->last_check = 0;
  this->next_check = 0;
  this->check_type = 0;
  this->last_state_change = 0;
  this->last_hard_state_change = 0;
  this->last_hard_state = 0;
  this->last_time_ok = 0;
  this->last_time_warning = 0;
  this->last_time_unknown = 0;
  this->last_time_critical = 0;
  this->state_type = 0;
  this->last_notification = 0;
  this->next_notification = 0;
  this->no_more_notifications = 0;
  this->problem_has_been_acknowledged = 0;
  this->acknowledgement_type = 0;
  this->current_notification_number = 0;
  this->passive_checks_enabled = 0;
  this->active_checks_enabled = 0;
  this->is_flapping = 0;
  this->percent_state_change = 0.0;
  this->latency = 0.0;
  this->execution_time = 0.0;
  this->scheduled_downtime_depth = 0;
  this->obsess_over_service = 0;
  this->modified_service_attributes = 0;
  this->normal_check_interval = 0.0;
  this->retry_check_interval = 0.0;
  this->check_timeperiod_object_id = 0;
}

/**
 *  ServiceStatusEvent copy constructor.
 */
ServiceStatusEvent::ServiceStatusEvent(const ServiceStatusEvent& sse) : StatusEvent()
{
  this->operator=(sse);
}

/**
 *  ServiceStatusEvent destructor.
 */
ServiceStatusEvent::~ServiceStatusEvent()
{
}

/**
 *  ServiceStatusEvent operator= overload.
 */
ServiceStatusEvent& ServiceStatusEvent::operator=(const ServiceStatusEvent& s)
{
  // Unfortunately, because of the std::strings we can't use memcpy.
  StatusEvent::operator=(s);
  this->host = s.host;
  this->service = s.service;
  this->status_update_time = s.status_update_time;
  this->output = s.output;
  this->perfdata = s.perfdata;
  this->current_state = s.current_state;
  this->has_been_checked = s.has_been_checked;
  this->should_be_scheduled = s.should_be_scheduled;
  this->current_check_attempt = s.current_check_attempt;
  this->max_check_attempts = s.max_check_attempts;
  this->last_check = s.last_check;
  this->next_check = s.next_check;
  this->check_type = s.check_type;
  this->last_state_change = s.last_state_change;
  this->last_hard_state_change = s.last_hard_state_change;
  this->last_hard_state = s.last_hard_state;
  this->last_time_ok = s.last_time_ok;
  this->last_time_warning = s.last_time_warning;
  this->last_time_unknown = s.last_time_unknown;
  this->last_time_critical = s.last_time_critical;
  this->state_type = s.state_type;
  this->last_notification = s.last_notification;
  this->next_notification = s.next_notification;
  this->no_more_notifications = s.no_more_notifications;
  this->problem_has_been_acknowledged = s.problem_has_been_acknowledged;
  this->acknowledgement_type = s.acknowledgement_type;
  this->current_notification_number = s.current_notification_number;
  this->passive_checks_enabled = s.passive_checks_enabled;
  this->active_checks_enabled = s.active_checks_enabled;
  this->is_flapping = s.is_flapping;
  this->percent_state_change = s.percent_state_change;
  this->latency = s.latency;
  this->execution_time = s.execution_time;
  this->scheduled_downtime_depth = s.scheduled_downtime_depth;
  this->obsess_over_service = s.obsess_over_service;
  this->modified_service_attributes = s.modified_service_attributes;
  this->normal_check_interval = s.normal_check_interval;
  this->retry_check_interval = s.retry_check_interval;
  this->check_timeperiod_object_id = s.check_timeperiod_object_id;
  return (*this);
}

/**
 *  Returns the type of the event.
 */
int ServiceStatusEvent::GetType() const throw ()
{
  // XXX : hardcoded value
  return (1);
}

/**
 *  Returns the host.
 */
const std::string& ServiceStatusEvent::GetHost() const throw ()
{
  return (this->host);
}

/**
 *  Returns the service.
 */
const std::string& ServiceStatusEvent::GetService() const throw ()
{
  return (this->service);
}

/**
 *  Returns the status_update_time.
 */
time_t ServiceStatusEvent::GetStatusUpdateTime() const throw ()
{
  return (this->status_update_time);
}

/**
 *  Returns the output.
 */
const std::string& ServiceStatusEvent::GetOutput() const throw ()
{
  return (this->output);
}

/**
 *  Returns the perfdata.
 */
const std::string& ServiceStatusEvent::GetPerfdata() const throw ()
{
  return (this->perfdata);
}

/**
 *  Returns the current_state.
 */
short ServiceStatusEvent::GetCurrentState() const throw ()
{
  return (this->current_state);
}

/**
 *  Returns the has_been_checked.
 */
short ServiceStatusEvent::GetHasBeenChecked() const throw ()
{
  return (this->has_been_checked);
}

/**
 *  Returns the should_be_scheduled.
 */
short ServiceStatusEvent::GetShouldBeScheduled() const throw ()
{
  return (this->should_be_scheduled);
}

/**
 *  Returns the current_check_attempt.
 */
short ServiceStatusEvent::GetCurrentCheckAttempt() const throw ()
{
  return (this->current_check_attempt);
}

/**
 *  Returns the max_check_attempts.
 */
short ServiceStatusEvent::GetMaxCheckAttempts() const throw ()
{
  return (this->max_check_attempts);
}

/**
 *  Returns the last_check.
 */
time_t ServiceStatusEvent::GetLastCheck() const throw ()
{
  return (this->last_check);
}

/**
 *  Returns the next_check.
 */
time_t ServiceStatusEvent::GetNextCheck() const throw ()
{
  return (this->next_check);
}

/**
 *  Returns the check_type.
 */
short ServiceStatusEvent::GetCheckType() const throw ()
{
  return (this->check_type);
}

/**
 *  Returns the last_state_change.
 */
time_t ServiceStatusEvent::GetLastStateChange() const throw ()
{
  return (this->last_state_change);
}

/**
 *  Returns the last_hard_state_change.
 */
time_t ServiceStatusEvent::GetLastHardStateChange() const throw ()
{
  return (this->last_hard_state_change);
}

/**
 *  Returns the last_hard_state.
 */
short ServiceStatusEvent::GetLastHardState() const throw ()
{
  return (this->last_hard_state);
}

/**
 *  Returns the last_time_ok.
 */
time_t ServiceStatusEvent::GetLastTimeOk() const throw ()
{
  return (this->last_time_ok);
}

/**
 *  Returns the last_time_warning.
 */
time_t ServiceStatusEvent::GetLastTimeWarning() const throw ()
{
  return (this->last_time_warning);
}

/**
 *  Returns the last_time_unknown.
 */
time_t ServiceStatusEvent::GetLastTimeUnknown() const throw ()
{
  return (this->last_time_unknown);
}

/**
 *  Returns the last_time_critical.
 */
time_t ServiceStatusEvent::GetLastTimeCritical() const throw ()
{
  return (this->last_time_critical);
}

/**
 *  Returns the state_type.
 */
short ServiceStatusEvent::GetStateType() const throw ()
{
  return (this->state_type);
}

/**
 *  Returns the last_notification.
 */
time_t ServiceStatusEvent::GetLastNotification() const throw ()
{
  return (this->last_notification);
}

/**
 *  Returns the next_notification.
 */
time_t ServiceStatusEvent::GetNextNotification() const throw ()
{
  return (this->next_notification);
}

/**
 *  Returns the no_more_notifications.
 */
short ServiceStatusEvent::GetNoMoreNotifications() const throw ()
{
  return (this->no_more_notifications);
}

/**
 *  Returns the problem_has_been_acknowledged.
 */
short ServiceStatusEvent::GetProblemHasBeenAcknowledged() const throw ()
{
  return (this->problem_has_been_acknowledged);
}

/**
 *  Returns the acknowledgement_type.
 */
short ServiceStatusEvent::GetAcknowledgementType() const throw ()
{
  return (this->acknowledgement_type);
}

/**
 *  Returns the current_notification_number.
 */
short ServiceStatusEvent::GetCurrentNotificationNumber() const throw ()
{
  return (this->current_notification_number);
}

/**
 *  Returns the passive_checks_enabled.
 */
short ServiceStatusEvent::GetPassiveChecksEnabled() const throw ()
{
  return (this->passive_checks_enabled);
}

/**
 *  Returns the active_checks_enabled.
 */
short ServiceStatusEvent::GetActiveChecksEnabled() const throw ()
{
  return (this->active_checks_enabled);
}

/**
 *  Returns the is_flapping.
 */
short ServiceStatusEvent::GetIsFlapping() const throw ()
{
  return (this->is_flapping);
}

/**
 *  Returns the percent_state_change.
 */
double ServiceStatusEvent::GetPercentStateChange() const throw ()
{
  return (this->percent_state_change);
}

/**
 *  Returns the latency.
 */
double ServiceStatusEvent::GetLatency() const throw ()
{
  return (this->latency);
}

/**
 *  Returns the execution_time.
 */
double ServiceStatusEvent::GetExecutionTime() const throw ()
{
  return (this->execution_time);
}

/**
 *  Returns the scheduled_downtime_depth.
 */
short ServiceStatusEvent::GetScheduledDowntimeDepth() const throw ()
{
  return (this->scheduled_downtime_depth);
}

/**
 *  Returns the obsess_over_service.
 */
short ServiceStatusEvent::GetObsessOverService() const throw ()
{
  return (this->obsess_over_service);
}

/**
 *  Returns the modified_service_attributes.
 */
int ServiceStatusEvent::GetModifiedServiceAttributes() const throw ()
{
  return (this->modified_service_attributes);
}

/**
 *  Returns the event_handler.
 */
const std::string& ServiceStatusEvent::GetEventHandler() const throw ()
{
  return (this->event_handler);
}

/**
 *  Returns the check_command.
 */
const std::string& ServiceStatusEvent::GetCheckCommand() const throw ()
{
  return (this->check_command);
}

/**
 *  Returns the normal_check_interval.
 */
double ServiceStatusEvent::GetNormalCheckInterval() const throw ()
{
  return (this->normal_check_interval);
}

/**
 *  Returns the retry_check_interval.
 */
double ServiceStatusEvent::GetRetryCheckInterval() const throw ()
{
  return (this->retry_check_interval);
}

/**
 *  Returns the check_timeperiod_object_id.
 */
int ServiceStatusEvent::GetCheckTimeperiodObjectId() const throw ()
{
  return (this->check_timeperiod_object_id);
}

/**
 *  Sets the host on which the event appeared.
 */
void ServiceStatusEvent::SetHost(const std::string& h)
{
  this->host = h;
  return ;
}

/**
 *  Sets the service to which this event refers.
 */
void ServiceStatusEvent::SetService(const std::string& s)
{
  this->service = s;
  return ;
}

/**
 *  Sets the status_update_time.
 */
void ServiceStatusEvent::SetStatusUpdateTime(time_t sut)
{
  this->status_update_time = sut;
  return ;
}

/**
 *  Sets the output.
 */
void ServiceStatusEvent::SetOutput(const std::string& o)
{
  this->output = o;
  return ;
}

/**
 *  Sets the perfdata.
 */
void ServiceStatusEvent::SetPerfdata(const std::string& p)
{
  this->perfdata = p;
  return ;
}

/**
 *  Sets the current_state.
 */
void ServiceStatusEvent::SetCurrentState(short cs)
{
  this->current_state = cs;
  return ;
}

/**
 *  Sets the has_been_checked.
 */
void ServiceStatusEvent::SetHasBeenChecked(short hbc)
{
  this->has_been_checked = hbc;
  return ;
}

/**
 *  Sets the should_be_scheduled.
 */
void ServiceStatusEvent::SetShouldBeScheduled(short sbs)
{
  this->should_be_scheduled = sbs;
  return ;
}

/**
 *  Sets the current_check_attempt.
 */
void ServiceStatusEvent::SetCurrentCheckAttempt(short cca)
{
  this->current_check_attempt = cca;
  return ;
}


/**
 *  Sets the max_check_attempts.
 */
void ServiceStatusEvent::SetMaxCheckAttempts(short mca)
{
  this->max_check_attempts = mca;
  return ;
}

/**
 *  Sets the last_check.
 */
void ServiceStatusEvent::SetLastCheck(time_t lc)
{
  this->last_check = lc;
  return ;
}

/**
 *  Sets the next_check.
 */
void ServiceStatusEvent::SetNextCheck(time_t nc)
{
  this->next_check = nc;
  return ;
}

/**
 *  Sets the check_type.
 */
void ServiceStatusEvent::SetCheckType(short ct)
{
  this->check_type = ct;
  return ;
}

/**
 *  Sets the last_state_change.
 */
void ServiceStatusEvent::SetLastStateChange(time_t lsc)
{
  this->last_state_change = lsc;
  return ;
}

/**
 *  Sets the last_hard_state_change.
 */
void ServiceStatusEvent::SetLastHardStateChange(time_t lhsc)
{
  this->last_hard_state_change = lhsc;
  return ;
}

/**
 *  Sets the last_hard_state.
 */
void ServiceStatusEvent::SetLastHardState(short lhs)
{
  this->last_hard_state = lhs;
  return ;
}

/**
 *  Sets the last_time_ok.
 */
void ServiceStatusEvent::SetLastTimeOk(time_t lto)
{
  this->last_time_ok = lto;
  return ;
}

/**
 *  Sets the last_time_warning.
 */
void ServiceStatusEvent::SetLastTimeWarning(time_t ltw)
{
  this->last_time_warning = ltw;
  return ;
}

/**
 *  Sets the last_time_unknown.
 */
void ServiceStatusEvent::SetLastTimeUnknown(time_t ltu)
{
  this->last_time_unknown = ltu;
  return ;
}

/**
 *  Sets the last_time_critical.
 */
void ServiceStatusEvent::SetLastTimeCritical(time_t ltc)
{
  this->last_time_critical = ltc;
  return ;
}

/**
 *  Sets the state_type.
 */
void ServiceStatusEvent::SetStateType(short st)
{
  this->state_type = st;
  return ;
}
/**
 *  Sets the last_notification.
 */
void ServiceStatusEvent::SetLastNotification(time_t ln)
{
  this->last_notification = ln;
  return ;
}

/**
 *  Sets the next_notification.
 */
void ServiceStatusEvent::SetNextNotification(time_t nn)
{
  this->next_notification = nn;
  return ;
}

/**
 *  Sets the no_more_notifications.
 */
void ServiceStatusEvent::SetNoMoreNotifications(short nmn)
{
  this->no_more_notifications = nmn;
  return ;
}

/**
 *  Sets the problem_has_been_acknowledged.
 */
void ServiceStatusEvent::SetProblemHasBeenAcknowledged(short phba)
{
  this->problem_has_been_acknowledged = phba;
  return ;
}

/**
 *  Sets the acknowledgement_type.
 */
void ServiceStatusEvent::SetAcknowledgementType(short at)
{
  this->acknowledgement_type = at;
  return ;
}

/**
 *  Sets the current_notification_number.
 */
void ServiceStatusEvent::SetCurrentNotificationNumber(short cnn)
{
  this->current_notification_number = cnn;
  return ;
}

/**
 *  Sets the passive_checks_enabled.
 */
void ServiceStatusEvent::SetPassiveChecksEnabled(short pce)
{
  this->passive_checks_enabled = pce;
  return ;
}

/**
 *  Sets the active_checks_enabled.
 */
void ServiceStatusEvent::SetActiveChecksEnabled(short ace)
{
  this->active_checks_enabled = ace;
  return ;
}

/**
 *  Sets the is_flapping.
 */
void ServiceStatusEvent::SetIsFlapping(short i_f)
{
  this->is_flapping = i_f;
  return ;
}

/**
 *  Sets the percent_state_change.
 */
void ServiceStatusEvent::SetPercentStateChange(double psc)
{
  this->percent_state_change = psc;
  return ;
}

/**
 *  Sets the latency.
 */
void ServiceStatusEvent::SetLatency(double l)
{
  this->latency = l;
  return ;
}

/**
 *  Sets the execution_time.
 */
void ServiceStatusEvent::SetExecutionTime(double et)
{
  this->execution_time = et;
  return ;
}

/**
 *  Sets the scheduled_downtime_depth.
 */
void ServiceStatusEvent::SetScheduledDowntimeDepth(short sdd)
{
  this->scheduled_downtime_depth = sdd;
  return ;
}

/**
 *  Sets the obsess_over_service.
 */
void ServiceStatusEvent::SetObsessOverService(short oos)
{
  this->obsess_over_service = oos;
  return ;
}

/**
 *  Sets the modified_service_attributes.
 */
void ServiceStatusEvent::SetModifiedServiceAttributes(int msa)
{
  this->modified_service_attributes = msa;
  return ;
}

/**
 *  Sets the event_handler.
 */
void ServiceStatusEvent::SetEventHandler(const std::string& eh)
{
  this->event_handler = eh;
  return ;
}

/**
 *  Sets the check_command.
 */
void ServiceStatusEvent::SetCheckCommand(const std::string& cc)
{
  this->check_command = cc;
  return ;
}

/**
 *  Sets the normal_check_interval.
 */
void ServiceStatusEvent::SetNormalCheckInterval(double nci)
{
  this->normal_check_interval = nci;
  return ;
}

/**
 *  Sets the retry_check_interval.
 */
void ServiceStatusEvent::SetRetryCheckInterval(double rci)
{
  this->retry_check_interval = rci;
  return ;
}

/**
 *  Sets the check_timeperiod_object_id.
 */
void ServiceStatusEvent::SetCheckTimeperiodObjectId(int ctoi)
{
  this->check_timeperiod_object_id = ctoi;
  return ;
}
