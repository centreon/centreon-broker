/*
** cb2db.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  06/01/09 Matthieu Kermagoret
** Last update 06/01/09 Matthieu Kermagoret
*/

#include "host.h"
#include "mapping.h"

using namespace CentreonBroker;

template <typename ObjectType,
          typename SuperType,
          typename ReturnType,
          ReturnType (SuperType::* method)() const>
ReturnType MethodToFunction(const ObjectType& object)
{
  return ((object.*method)());
}

static MappedField<Host> hosts[] =
  {
    {
      "acknowledgement_type",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetAcknowledgementType>)
    },
    {
      "active_checks_enabled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetActiveChecksEnabled>)
    },
    {
      "check_command",
      DynamicFieldSetter<Host, const std::string&>(
        &MethodToFunction<Host, HostServiceStatus, const std::string&,
                          &HostServiceStatus::GetCheckCommand>)
    },
    {
      "check_interval",
      DynamicFieldSetter<Host, double>(
        &MethodToFunction<Host, HostServiceStatus, double,
                          &HostServiceStatus::GetCheckInterval>)
    },
    {
      "check_type",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetCheckType>)
    },
    {
      "current_check_attempt",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetCurrentCheckAttempt>)
    },
    {
      "current_notification_number",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetCurrentNotificationNumber>)
    },
    {
      "current_state",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetCurrentState>)
    },
    {
      "event_handler",
      DynamicFieldSetter<Host, const std::string&>(
        &MethodToFunction<Host, HostServiceStatus, const std::string&,
                          &HostServiceStatus::GetEventHandler>)
    },
    {
      "event_handler_enabled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, Status, short,
                          &Status::GetEventHandlerEnabled>)
    },
    {
      "execution_time",
      DynamicFieldSetter<Host, double>(
        &MethodToFunction<Host, HostServiceStatus, double,
                          &HostServiceStatus::GetExecutionTime>)
    },
    {
      "failure_prediction_enabled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, Status, short,
                          &Status::GetFailurePredictionEnabled>)
    },
    {
      "flap_detection_enabled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, Status, short,
                          &Status::GetFlapDetectionEnabled>)
    },
    {
      "has_been_checked",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetHasBeenChecked>)
    },
    {
      "host_name",
      DynamicFieldSetter<Host, const std::string&>(
        &MethodToFunction<Host, HostServiceStatus, const std::string&,
                          &HostServiceStatus::GetHostName>)
    },
    {
      "is_flapping",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetIsFlapping>)
    },
    {
      "last_check",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, HostServiceStatus, time_t,
                          &HostServiceStatus::GetLastCheck>)
    },
    {
      "last_hard_state",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetLastHardState>)
    },
    {
      "last_hard_state_change",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, HostServiceStatus, time_t,
                          &HostServiceStatus::GetLastHardStateChange>)
    },
    {
      "last_notification",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, HostServiceStatus, time_t,
                          &HostServiceStatus::GetLastNotification>)
    },
    {
      "last_state_change",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, HostServiceStatus, time_t,
                          &HostServiceStatus::GetLastStateChange>)
    },
    {
      "latency",
      DynamicFieldSetter<Host, double>(
        &MethodToFunction<Host, HostServiceStatus, double,
                          &HostServiceStatus::GetLatency>)
    },
    {
      "max_check_attempts",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetMaxCheckAttempts>)
    },
    {
      "modified_host_attributes",
      DynamicFieldSetter<Host, int>(
        &MethodToFunction<Host, HostServiceStatus, int,
                          &HostServiceStatus::GetModifiedAttributes>)
    },
    {
      "next_check",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, HostServiceStatus, time_t,
                          &HostServiceStatus::GetNextCheck>)
    },
    {
      "next_notification",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, HostServiceStatus, time_t,
                          &HostServiceStatus::GetNextNotification>)
    },
    {
      "no_more_notifications",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetNoMoreNotifications>)
    },
    {
      "notifications_enabled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, Status, short,
                          &Status::GetNotificationsEnabled>)
    },
    {
      "obsess_over_host",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetObsessOver>)
    },
    {
      "output",
      DynamicFieldSetter<Host, const std::string&>(
        &MethodToFunction<Host, HostServiceStatus, const std::string&,
                          &HostServicestatus::GetOutput>)
    },
    {
      "passive_checks_enabled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetPassiveChecksEnabled>)
    },
    {
      "percent_state_change",
      DynamicFieldSetter<Host, double>(
        &MethodToFunction<Host, HostServiceStatus, double,
                          &HostServiceStatus::GetPercentStateChange>)
    },
    {
      "perfdata",
      DynamicFieldSetter<Host, const std::string&>(
        &MethodToFunction<Host, HostServiceStatus, const std::string&,
                          &HostServiceStatus::GetPerfdata>)
    },
    {
      "problem_has_been_acknowledged",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetProblemHasBeenAcknowledged>)
    },
    {
      "process_performance_data",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, Status, short,
                          &Status::GetProcessPerformanceData>)
    },
    {
      "retry_interval",
      DynamicFieldSetter<Host, double>(
        &MethodToFunction<Host, HostServiceStatus, double,
                          &HostServiceStatus::GetRetryInterval>)
    },
    {
      "scheduled_downtime_depth",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetScheduledDowntimeDepth>)
    },
    {
      "should_be_scheduled",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetShouldBeScheduled>)
    },
    {
      "state_type",
      DynamicFieldSetter<Host, short>(
        &MethodToFunction<Host, HostServiceStatus, short,
                          &HostServiceStatus::GetStateType>)
    },
    {
      "status_update_time",
      DynamicFieldSetter<Host, time_t>(
        &MethodToFunction<Host, Status, time_t,
                          &Status::GetStatusUpdateTime>)
    }
  };
