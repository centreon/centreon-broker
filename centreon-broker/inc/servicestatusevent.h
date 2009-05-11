/*
** servicestatusevent.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/11/09 Matthieu Kermagoret
*/

#ifndef SERVICESTATUSEVENT_H_
# define SERVICESTATUSEVENT_H_

# include <string>
# include "event.h"

namespace               CentreonBroker
{
  class                 EventSubscriber;

  class                 ServiceStatusEvent : public Event
  {
   private:
    int                 servicestatus_id;
    short               instance_id;
    int                 service_object_id;
    time_t              status_update_time;
    std::string         output;
    std::string         perfdata;
    short               current_state;
    short               has_been_checked;
    short               should_be_scheduled;
    short               current_check_attempt;
    short               max_check_attempts;
    time_t              last_check;
    time_t              next_check;
    short               check_type;
    time_t              last_state_change;
    time_t              last_hard_state_change;
    short               last_hard_state;
    time_t              last_time_ok;
    time_t              last_time_warning;
    time_t              last_time_unknown;
    time_t              last_time_critical;
    short               state_type;
    time_t              last_notification;
    time_t              next_notification;
    short               no_more_notifications;
    short               notifications_enabled;
    short               problem_has_been_acknowledged;
    short               acknowledgement_type;
    short               current_notification_number;
    short               passive_checks_enabled;
    short               active_checks_enabled;
    short               event_handler_enabled;
    short               flap_detection_enabled;
    short               is_flapping;
    double              percent_state_change;
    double              latency;
    double              execution_time;
    short               scheduled_downtime_depth;
    short               failure_prediction_enabled;
    short               process_performance_data;
    short               obsess_over_service;
    int                 modified_service_attributes;
    std::string         event_handler;
    std::string         check_command;
    double              normal_check_interval;
    double              retry_check_interval;
    int                 check_timeperiod_object_id;

   public:
                        ServiceStatusEvent();
                        ServiceStatusEvent(const ServiceStatusEvent& sse);
                        ~ServiceStatusEvent();
    ServiceStatusEvent& operator=(const ServiceStatusEvent& sse);
    void                AcceptVisitor(EventSubscriber& es);
    int                 GetType() const;
    void                SetServiceStatusId(int ssi);
    void                SetInstanceId(short ii);
    void                SetServiceObjectId(int soi);
    void                SetStatusUpdateTime(time_t sut);
    void                SetOutput(const std::string& o);
    void                SetPerfdata(const std::string& p);
    void                SetCurrentState(short cs);
    void                SetHasBeenChecked(short hbc);
    void                SetShouldBeScheduled(short sbs);
    void                SetCurrentCheckAttempt(short cca);
    void                SetMaxCheckAttempts(short mca);
    void                SetLastCheck(time_t lc);
    void                SetNextCheck(time_t nc);
    void                SetCheckType(short ct);
    void                SetLastStateChange(time_t lsc);
    void                SetLastHardStateChange(time_t lhsc);
    void                SetLastHardState(short lhs);
    void                SetLastTimeOk(time_t lto);
    void                SetLastTimeWarning(time_t ltw);
    void                SetLastTimeUnknown(time_t ltu);
    void                SetLastTimeCritical(time_t ltc);
    void                SetStateType(short st);
    void                SetLastNotification(time_t ln);
    void                SetNextNotification(time_t nn);
    void                SetNoMoreNotifications(short nmn);
    void                SetNotificationsEnabled(short ne);
    void                SetProblemHasBeenAcknowledged(short phba);
    void                SetAcknowledgementType(short at);
    void                SetCurrentNotificationNumber(short cnb);
    void                SetPassiveChecksEnabled(short pce);
    void                SetActiveChecksEnabled(short ace);
    void                SetEventHandlerEnabled(short ehe);
    void                SetFlapDetectionEnabled(short fde);
    void                SetIsFlapping(short i_f);
    void                SetPercentStateChange(double psc);
    void                SetLatency(double l);
    void                SetExecutionTime(double et);
    void                SetScheduledDowntimeDepth(short sdd);
    void                SetFailurePredictionEnabled(short fpe);
    void                SetProcessPerformanceData(short ppd);
    void                SetObsessOverService(short oos);
    void                SetModifiedServiceAttributes(int msa);
    void                SetEventHandler(const std::string& eh);
    void                SetCheckCommand(const std::string& cc);
    void                SetNormalCheckInterval(double nci);
    void                SetRetryCheckInterval(double rci);
    void                SetCheckTimeperiodObjectId(int ctoi);
  };
}

#endif /* !SERVICESTATUSEVENT_H_ */
