/*
** program_status_event.h for CentreonBroker in ./inc
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/20/09 Matthieu Kermagoret
** Last update 05/20/09 Matthieu Kermagoret
*/

#ifndef PROGRAM_STATUS_EVENT_H_
# define PROGRAM_STATUS_EVENT_H_

namespace CentreonBroker
{
  class ProgramStatusEvent
  {
    enum Int
    {
      MODIFIED_HOST_ATTRIBUTES,
      MODIFIED_SERVICE_ATTRIBUTES,
      PROCESS_ID
    };
    enum Short
    {
      ACTIVE_SERVICE_CHECKS_ENABLED,
      PASSIVE_SERVICE_CHECKS_ENABLED,
      ACTIVE_HOST_CHECKS_ENABLED
      PASSIVE_HOST_CHECKS_ENABLED
      OBSESS_OVER_HOSTS
      OBSESS_OVER_SERVICES
      DAEMON_MODE,
      IS_CURRENTLY_RUNNING,
    };
    enum String
    {
      GLOBAL_HOST_EVENT_HANDLER,
      GLOBAL_SERVICE_EVENT_HANDLER
    };
    enum TimeT
    {
      LAST_COMMAND_CHECK,
      LAST_LOG_ROTATION,
      PROGRAM_START_TIME,
      PROGRAM_END_TIME,
    };
  };
}

#endif /* !PROGRAM_STATUS_EVENT_H_ */
