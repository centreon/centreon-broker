/*
** networkinput.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 05/11/09 Matthieu Kermagoret
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "eventpublisher.h"
#include "networkinput.h"
#include "servicestatusevent.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

NetworkInput::NetworkInput(const NetworkInput& ni) : Thread()
{
  (void)ni;
}

NetworkInput& NetworkInput::operator=(const NetworkInput& ni)
{
  (void)ni;
  return (*this);
}

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

NetworkInput::NetworkInput()
{
  this->fd = -1;
}

NetworkInput::~NetworkInput()
{
  if (this->fd >= 0)
    {
      close(this->fd);
      this->Join();
    }
}

int NetworkInput::Core()
{
  char buffer[2048];
  FILE* stream;
  static int id = 1;

  // XXX : those are test stuff
  stream = fdopen(this->fd, "r+");
  while (fgets(buffer, sizeof(buffer), stream))
    {
      std::cout << buffer;
      if (!strcmp(buffer, "213:\n")) // service status
	{
	  ServiceStatusEvent *sse = new ServiceStatusEvent;

	  sse->SetServiceStatusId(id++);
	  sse->SetServiceObjectId(id++);
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_TYPE
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_FLAGS
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_ATTRIBUTES
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_TIMESTAMP
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_HOST
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_SERVICE
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_OUTPUT
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetOutput(strchr(buffer, '=') + 1);
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_PERFDATA
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetPerfdata(strchr(buffer, '=') + 1);
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_CURRENTSTATE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetCurrentState(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_HASBEENCHECKED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetHasBeenChecked(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_SHOULDBESCHEDULED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetShouldBeScheduled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_CURRENTCHECKATTTEMPT
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetCurrentCheckAttempt(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_MAXCHECKATTEMPTS
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetMaxCheckAttempts(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTSERVICECHECK
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastCheck(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_NEXTSERVICECHECK
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetNextCheck(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_CHECKTYPE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetCheckType(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTSTATECHANGE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastStateChange(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTHARDSTATECHANGE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastHardStateChange(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTHARDSTATE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastHardState(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTTIMEOK
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastTimeOk(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTTIMEWARNING
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastTimeWarning(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTTIMEUNKNOWN
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastTimeUnknown(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTTIMECRITICAL
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastTimeCritical(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_STATETYPE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetStateType(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LASTSERVICENOTIFICATION
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLastNotification(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_NEXTSERVICENOTIFICATION
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetNextNotification(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_NOMORENOTIFICATIONS
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetNoMoreNotifications(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_NOTIFICATIONSENABLED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetNotificationsEnabled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_PROBLEMHASBEENACKNOWLEDGED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetProblemHasBeenAcknowledged(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_ACKNOWLEDGEMENTTYPE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetAcknowledgementType(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_CURRENTNOTIFICATIONUMBER
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetCurrentNotificationNumber(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_PASSIVESERVICECHECKSENABLED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetPassiveChecksEnabled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_EVENTHANDLERENABLED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetEventHandlerEnabled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_ACTIVESERVICECHECKSENABLED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetActiveChecksEnabled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_FLAPDETECTIONENABLED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetFlapDetectionEnabled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_ISFLAPPING
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetIsFlapping(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_PERCENTSTATECHANGE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetPercentStateChange(strtod(strchr(buffer, '=') + 1, NULL));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_LATENCY
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetLatency(strtod(strchr(buffer, '=') + 1, NULL));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_EXECUTIONTIME
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetExecutionTime(strtod(strchr(buffer, '=') + 1, NULL));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_SCHEDULEDDOWNTIMEDEPTH
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetScheduledDowntimeDepth(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_FAILUREPREDICTIONENABLED
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetFailurePredictionEnabled(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_PROCESSPERFORMANCEDATA
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetProcessPerformanceData(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_OBSESSOVERSERVICE
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetObsessOverService(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_MODIFIEDSERVICEATTRIBUTES
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetModifiedServiceAttributes(atoi(strchr(buffer, '=') + 1));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_EVENTHANDLER
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetEventHandler(strchr(buffer, '=') + 1);
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_CHECKCOMMAND
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetCheckCommand(strchr(buffer, '=') + 1);
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_NORMALCHECKINTERVAL
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetNormalCheckInterval(strtod(strchr(buffer, '=') + 1, NULL));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_RETRYCHECKINTERVAL
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetRetryCheckInterval(strtod(strchr(buffer, '=') + 1, NULL));
	  fgets(buffer, sizeof(buffer), stream); // NDO_DATA_SERVICECHECKPERIOD
	  buffer[strlen(buffer) - 1] = '\0';
	  sse->SetCheckTimeperiodObjectId(atoi(strchr(buffer, '=') + 1));
	  EventPublisher::GetInstance()->Publish(sse);
	}
    }
  fclose(stream);
  return (0);
}

void NetworkInput::SetFD(int fd)
{
  this->fd = fd;
  this->Run();
  return ;
}
