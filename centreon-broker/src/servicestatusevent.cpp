/*
** servicestatusevent.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/13/09 Matthieu Kermagoret
*/

#include "eventsubscriber.h"
#include "servicestatusevent.h"

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
  this->servicestatus_id = 0;
  this->instance_id = 0;
  this->service_object_id = 0;
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
  this->notifications_enabled = 0;
  this->problem_has_been_acknowledged = 0;
  this->acknowledgement_type = 0;
  this->current_notification_number = 0;
  this->passive_checks_enabled = 0;
  this->active_checks_enabled = 0;
  this->event_handler_enabled = 0;
  this->flap_detection_enabled = 0;
  this->is_flapping = 0;
  this->percent_state_change = 0.0;
  this->latency = 0.0;
  this->execution_time = 0.0;
  this->scheduled_downtime_depth = 0;
  this->failure_prediction_enabled = 0;
  this->process_performance_data = 0;
  this->obsess_over_service = 0;
  this->modified_service_attributes = 0;
  this->normal_check_interval = 0.0;
  this->retry_check_interval = 0.0;
  this->check_timeperiod_object_id = 0;
}

/**
 *  ServiceStatusEvent copy constructor.
 */
ServiceStatusEvent::ServiceStatusEvent(const ServiceStatusEvent& sse) : Event()
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
  this->servicestatus_id = s.servicestatus_id;
  this->instance_id = s.instance_id;
  this->service_object_id = s.service_object_id;
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
  this->notifications_enabled = s.notifications_enabled;
  this->problem_has_been_acknowledged = s.problem_has_been_acknowledged;
  this->acknowledgement_type = s.acknowledgement_type;
  this->current_notification_number = s.current_notification_number;
  this->passive_checks_enabled = s.passive_checks_enabled;
  this->active_checks_enabled = s.active_checks_enabled;
  this->event_handler_enabled = s.event_handler_enabled;
  this->flap_detection_enabled = s.flap_detection_enabled;
  this->is_flapping = s.is_flapping;
  this->percent_state_change = s.percent_state_change;
  this->latency = s.latency;
  this->execution_time = s.execution_time;
  this->scheduled_downtime_depth = s.scheduled_downtime_depth;
  this->failure_prediction_enabled = s.failure_prediction_enabled;
  this->process_performance_data = s.process_performance_data;
  this->obsess_over_service = s.obsess_over_service;
  this->modified_service_attributes = s.modified_service_attributes;
  this->normal_check_interval = s.normal_check_interval;
  this->retry_check_interval = s.retry_check_interval;
  this->check_timeperiod_object_id = s.check_timeperiod_object_id;
  return (*this);
}

/**
 *  Take the visitor and show him the inside.
 */
void ServiceStatusEvent::AcceptVisitor(EventSubscriber& es)
{
  es.Visit(this->servicestatus_id);
  es.Visit(this->instance_id);
  es.Visit(this->service_object_id);
  es.Visit(this->status_update_time);
  es.Visit(this->output);
  es.Visit(this->perfdata);
  es.Visit(this->current_state);
  es.Visit(this->has_been_checked);
  es.Visit(this->should_be_scheduled);
  es.Visit(this->current_check_attempt);
  es.Visit(this->max_check_attempts);
  es.Visit(this->last_check);
  es.Visit(this->next_check);
  es.Visit(this->check_type);
  es.Visit(this->last_state_change);
  es.Visit(this->last_hard_state_change);
  es.Visit(this->last_hard_state);
  es.Visit(this->last_time_ok);
  es.Visit(this->last_time_warning);
  es.Visit(this->last_time_unknown);
  es.Visit(this->last_time_critical);
  es.Visit(this->state_type);
  es.Visit(this->last_notification);
  es.Visit(this->next_notification);
  es.Visit(this->no_more_notifications);
  es.Visit(this->notifications_enabled);
  es.Visit(this->problem_has_been_acknowledged);
  es.Visit(this->acknowledgement_type);
  es.Visit(this->current_notification_number);
  es.Visit(this->passive_checks_enabled);
  es.Visit(this->active_checks_enabled);
  es.Visit(this->event_handler_enabled);
  es.Visit(this->flap_detection_enabled);
  es.Visit(this->is_flapping);
  es.Visit(this->percent_state_change);
  es.Visit(this->latency);
  es.Visit(this->execution_time);
  es.Visit(this->scheduled_downtime_depth);
  es.Visit(this->failure_prediction_enabled);
  es.Visit(this->process_performance_data);
  es.Visit(this->obsess_over_service);
  es.Visit(this->modified_service_attributes);
  es.Visit(this->event_handler);
  es.Visit(this->check_command);
  es.Visit(this->normal_check_interval);
  es.Visit(this->retry_check_interval);
  es.Visit(this->check_timeperiod_object_id);
  return ;
}

/**
 *  Returns the type of the event.
 */
int ServiceStatusEvent::GetType() const
{
  // XXX : hardcoded value
  return (1);
}

/**
 *  Sets the servicestatus_id.
 */
void ServiceStatusEvent::SetServiceStatusId(int ssi)
{
  this->servicestatus_id = ssi;
  return ;
}

/**
 *  Sets the instance_id.
 */
void ServiceStatusEvent::SetInstanceId(short ii)
{
  this->instance_id = ii;
  return ;
}

/**
 *  Sets the host_object_id.
 */
void ServiceStatusEvent::SetServiceObjectId(int soi)
{
  this->service_object_id = soi;
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
 *  Sets the notifications_enabled.
 */
void ServiceStatusEvent::SetNotificationsEnabled(short ne)
{
  this->notifications_enabled = ne;
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
 *  Sets the event_handler_enabled.
 */
void ServiceStatusEvent::SetEventHandlerEnabled(short ehe)
{
  this->event_handler_enabled = ehe;
  return ;
}

/**
 *  Sets the flap_detection_enabled.
 */
void ServiceStatusEvent::SetFlapDetectionEnabled(short fde)
{
  this->flap_detection_enabled = fde;
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
 *  Sets the failure_prediction_enabled.
 */
void ServiceStatusEvent::SetFailurePredictionEnabled(short fpe)
{
  this->failure_prediction_enabled = fpe;
  return ;
}

/**
 *  Sets the process_performance_data.
 */
void ServiceStatusEvent::SetProcessPerformanceData(short ppd)
{
  this->process_performance_data = ppd;
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
