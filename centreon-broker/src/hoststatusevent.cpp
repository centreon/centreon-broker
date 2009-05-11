/*
** hoststatusevent.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/07/09 Matthieu Kermagoret
** Last update 05/11/09 Matthieu Kermagoret
*/

#include <cstring>
#include "eventsubscriber.h"
#include "hoststatusevent.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HostStatusEvent constructor.
 */
HostStatusEvent::HostStatusEvent()
{
}

/**
 *  HostStatusEvent copy constructor.
 */
HostStatusEvent::HostStatusEvent(const HostStatusEvent& hse) : Event()
{
  operator=(hse);
}

/**
 *  HostStatusEvent destructor.
 */
HostStatusEvent::~HostStatusEvent()
{
}

/**
 *  HostStatusEvent operator= overload.
 */
HostStatusEvent& HostStatusEvent::operator=(const HostStatusEvent& hse)
{
  // XXX : memcpy is not a proper way
  return (*this);
}

/**
 *  Take the visitor and show him the inside.
 */
void HostStatusEvent::AcceptVisitor(EventSubscriber& ev)
{
  ev.Visit(this->hoststatus_id);
  ev.Visit(this->instance_id);
  ev.Visit(this->host_object_id);
  ev.Visit(this->status_update_time);
  ev.Visit(this->output);
  ev.Visit(this->perfdata);
  ev.Visit(this->current_state);
  ev.Visit(this->has_been_checked);
  ev.Visit(this->should_be_scheduled);
  ev.Visit(this->current_check_attempt);
  ev.Visit(this->max_check_attempts);
  ev.Visit(this->last_check);
  ev.Visit(this->next_check);
  ev.Visit(this->check_type);
  ev.Visit(this->last_state_change);
  ev.Visit(this->last_hard_state_change);
  ev.Visit(this->last_time_up);
  ev.Visit(this->last_time_down);
  ev.Visit(this->last_time_unreachable);
  ev.Visit(this->state_type);
  ev.Visit(this->last_notification);
  ev.Visit(this->next_notification);
  ev.Visit(this->no_more_notifications);
  ev.Visit(this->notifications_enabled);
  ev.Visit(this->problem_has_been_acknowledged);
  ev.Visit(this->acknowledgement_type);
  ev.Visit(this->current_notification_number);
  ev.Visit(this->passive_checks_enabled);
  ev.Visit(this->active_checks_enabled);
  ev.Visit(this->event_handler_enabled);
  ev.Visit(this->flap_detection_enabled);
  ev.Visit(this->is_flapping);
  ev.Visit(this->percent_state_change);
  ev.Visit(this->latency);
  ev.Visit(this->execution_time);
  ev.Visit(this->scheduled_downtime_depth);
  ev.Visit(this->failure_prediction_enabled);
  ev.Visit(this->process_performance_data);
  ev.Visit(this->obsess_over_host);
  ev.Visit(this->modified_host_attributes);
  ev.Visit(this->event_handler);
  ev.Visit(this->check_command);
  ev.Visit(this->normal_check_interval);
  ev.Visit(this->retry_check_interval);
  ev.Visit(this->check_timeperiod_object_id);
  return ;
}

/**
 *  Returns the type of the event.
 */
int HostStatusEvent::GetType() const
{
  return (0);
}

/**
 *  Sets the hoststatus_id.
 */
void HostStatusEvent::SetHostStatusId(int hsi)
{
  this->hoststatus_id = hsi;
  return ;
}

/**
 *  Sets the instance_id.
 */
void HostStatusEvent::SetInstanceId(short ii)
{
  this->instance_id = ii;
  return ;
}

/**
 *  Sets the host_object_id.
 */
void HostStatusEvent::SetHostObjectId(int hoi)
{
  this->host_object_id = hoi;
  return ;
}

/**
 *  Sets the status_update_time.
 */
void HostStatusEvent::SetStatusUpdateTime(time_t sut)
{
  this->status_update_time = sut;
  return ;
}

/**
 *  Sets the output.
 */
void HostStatusEvent::SetOutput(const std::string& o)
{
  this->output = o;
  return ;
}

/**
 *  Sets the perfdata.
 */
void HostStatusEvent::SetPerfdata(const std::string& p)
{
  this->perfdata = p;
  return ;
}

/**
 *  Sets the current_state.
 */
void HostStatusEvent::SetCurrentState(short cs)
{
  this->current_state = cs;
  return ;
}

/**
 *  Sets the has_been_checked.
 */
void HostStatusEvent::SetHasBeenChecked(short hbc)
{
  this->has_been_checked = hbc;
  return ;
}

/**
 *  Sets the should_be_scheduled.
 */
void HostStatusEvent::SetShouldBeScheduled(short sbs)
{
  this->should_be_scheduled = sbs;
  return ;
}

/**
 *  Sets the current_check_attempt.
 */
void HostStatusEvent::SetCurrentCheckAttempt(short cca)
{
  this->current_check_attempt = cca;
  return ;
}

/**
 *  Sets the max_check_attempts.
 */
void HostStatusEvent::SetMaxCheckAttempts(short mca)
{
  this->max_check_attempts = mca;
  return ;
}

/**
 *  Sets the last_check.
 */
void HostStatusEvent::SetLastCheck(time_t lc)
{
  this->last_check = lc;
  return ;
}

/**
 *  Sets the next_check.
 */
void HostStatusEvent::SetNextCheck(time_t nc)
{
  this->next_check = nc;
  return ;
}

/**
 *  Sets the check_type.
 */
void HostStatusEvent::SetCheckType(short ct)
{
  this->check_type = ct;
  return ;
}

/**
 *  Sets the last_state_change.
 */
void HostStatusEvent::SetLastStateChange(time_t lsc)
{
  this->last_state_change = lsc;
  return ;
}

/**
 *  Sets the last_hard_state_change.
 */
void HostStatusEvent::SetLastHardStateChange(time_t lhsc)
{
  this->last_hard_state_change = lhsc;
  return ;
}

/**
 *  Sets the last_time_up.
 */
void HostStatusEvent::SetLastTimeUp(time_t ltu)
{
  this->last_time_up = ltu;
  return ;
}

/**
 *  Sets the last_time_down.
 */
void HostStatusEvent::SetLastTimeDown(time_t ltd)
{
  this->last_time_down = ltd;
  return ;
}

/**
 *  Sets the last_time_unreachable.
 */
void HostStatusEvent::SetLastTimeUnreachable(time_t ltu)
{
  this->last_time_unreachable = ltu;
  return ;
}

/**
 *  Sets the state_type.
 */
void HostStatusEvent::SetStateType(short st)
{
  this->state_type = st;
  return ;
}

/**
 *  Sets the last_notification.
 */
void HostStatusEvent::SetLastNotification(time_t ln)
{
  this->last_notification = ln;
  return ;
}

/**
 *  Sets the next_notification.
 */
void HostStatusEvent::SetNextNotification(time_t nn)
{
  this->next_notification = nn;
  return ;
}

/**
 *  Sets the no_more_notifications.
 */
void HostStatusEvent::SetNoMoreNotifications(short nmn)
{
  this->no_more_notifications = nmn;
  return ;
}

/**
 *  Sets the notifications_enabled.
 */
void HostStatusEvent::SetNotificationsEnabled(short ne)
{
  this->notifications_enabled = ne;
  return ;
}

/**
 *  Sets the problem_has_been_acknowledged.
 */
void HostStatusEvent::SetProblemHasBeenAcknowledged(short phba)
{
  this->problem_has_been_acknowledged = phba;
  return ;
}

/**
 *  Sets the acknowledgement_type.
 */
void HostStatusEvent::SetAcknowledgementType(short at)
{
  this->acknowledgement_type = at;
  return ;
}

/**
 *  Sets the current_notification_number.
 */
void HostStatusEvent::SetCurrentNotificationNumber(short cnn)
{
  this->current_notification_number = cnn;
  return ;
}

/**
 *  Sets the passive_checks_enabled.
 */
void HostStatusEvent::SetPassiveChecksEnabled(short pce)
{
  this->passive_checks_enabled = pce;
  return ;
}

/**
 *  Sets the active_checks_enabled.
 */
void HostStatusEvent::SetActiveChecksEnabled(short ace)
{
  this->active_checks_enabled = ace;
  return ;
}

/**
 *  Sets the event_handler_enabled.
 */
void HostStatusEvent::SetEventHandlerEnabled(short ehe)
{
  this->event_handler_enabled = ehe;
  return ;
}

/**
 *  Sets the flap_detection_enabled.
 */
void HostStatusEvent::SetFlapDetectionEnabled(short fde)
{
  this->flap_detection_enabled = fde;
  return ;
}

/**
 *  Sets the is_flapping.
 */
void HostStatusEvent::SetIsFlapping(short i_f)
{
  this->is_flapping = i_f;
  return ;
}

/**
 *  Sets the percent_state_change.
 */
void HostStatusEvent::SetPercentStateChange(double psc)
{
  this->percent_state_change = psc;
  return ;
}

/**
 *  Sets the latency.
 */
void HostStatusEvent::SetLatency(double l)
{
  this->latency = l;
  return ;
}

/**
 *  Sets the execution_time.
 */
void HostStatusEvent::SetExecutionTime(double et)
{
  this->execution_time = et;
  return ;
}

/**
 *  Sets the scheduled_downtime_depth.
 */
void HostStatusEvent::SetScheduledDowntimeDepth(short sdd)
{
  this->scheduled_downtime_depth = sdd;
  return ;
}

/**
 *  Sets the failure_prediction_enabled.
 */
void HostStatusEvent::SetFailurePredictionEnabled(short fpe)
{
  this->failure_prediction_enabled = fpe;
  return ;
}

/**
 *  Sets the process_performance_data.
 */
void HostStatusEvent::SetProcessPerformanceData(short ppd)
{
  this->process_performance_data = ppd;
  return ;
}

/**
 *  Sets the obsess_over_host.
 */
void HostStatusEvent::SetObsessOverHost(short ooh)
{
  this->obsess_over_host = ooh;
  return ;
}

/**
 *  Sets the modified_host_attributes.
 */
void HostStatusEvent::SetModifiedHostAttributes(int mha)
{
  this->modified_host_attributes = mha;
  return ;
}

/**
 *  Sets the event_handler.
 */
void HostStatusEvent::SetEventHandler(const std::string& eh)
{
  this->event_handler = eh;
  return ;
}

/**
 *  Sets the check_command.
 */
void HostStatusEvent::SetCheckCommand(const std::string& cc)
{
  this->check_command = cc;
  return ;
}

/**
 *  Sets the normal_check_interval.
 */
void HostStatusEvent::SetNormalCheckInterval(double nci)
{
  this->normal_check_interval = nci;
  return ;
}

/**
 *  Sets the retry_check_interval.
 */
void HostStatusEvent::SetRetryCheckInterval(double rci)
{
  this->retry_check_interval = rci;
  return ;
}

/**
 *  Sets the check_timeperiod_object_id.
 */
void HostStatusEvent::SetCheckTimeperiodObjectId(int ctoi)
{
  this->check_timeperiod_object_id = ctoi;
  return ;
}
