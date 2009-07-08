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

#include <boost/thread/mutex.hpp>
#include <cstdlib>
#include <ctime>
#include "event_publisher.h"
#include "events/acknowledgement.h"
#include "events/comment.h"
#include "events/connection.h"
#include "events/downtime.h"
#include "events/host.h"
#include "events/host_group.h"
#include "events/host_status.h"
#include "events/program_status.h"
#include "events/service.h"
#include "events/service_status.h"
#include "logging.h"
#include "nagios/protoapi.h"
#include "network_input.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Static objects            *
*                                     *
**************************************/

static boost::mutex             gl_mutex;
static std::list<NetworkInput*> gl_ni;

static void networkinput_destruction() throw ()
{
  try
    {
      boost::unique_lock<boost::mutex> lock(gl_mutex);

      while (!gl_ni.empty())
        {
          std::list<NetworkInput*>::iterator it;

          it = gl_ni.begin();
          lock.unlock();
          delete (*it);
          lock.lock();
        }
    }
  catch (...)
    {
    }
  return ;
}

namespace    CentreonBroker
{
  struct     NetworkInputDestructionRegistration
  {
             NetworkInputDestructionRegistration() throw ()
    {
      atexit(networkinput_destruction);
    }
  };
}

/**
 *  This fake static object is used to register at startup the NetworkInput
 *  cleanup function that will run on termination.
 */
static CentreonBroker::NetworkInputDestructionRegistration gl_nidr;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  This structure is used by the HandleObject template function.
 */
template <typename Event>
struct KeySetter
{
  int key;
  char type;
  union UHandler
  {
   public:
    void (Event::* set_bool)(bool);
    void (Event::* set_double)(double);
    void (Event::* set_int)(int);
    void (Event::* set_short)(short);
    void (Event::* set_string)(const std::string&);
    void (Event::* set_timet)(time_t);

    UHandler() : set_bool(NULL) {}
    UHandler(void (Event::* sb)(bool)) : set_bool(sb) {}
    UHandler(void (Event::* sd)(double)) : set_double(sd) {}
    UHandler(void (Event::* si)(int)) : set_int(si) {}
    UHandler(void (Event::* ss)(short)) : set_short(ss) {}
    UHandler(void (Event::* ss)(const std::string&)) : set_string(ss) {}
    UHandler(void (Event::* st)(time_t)) : set_timet(st) {}
  } setter;
};

/**
 *  For all kind of events, this template function will parse the socket input,
 *  fill the object and publish it.
 */
template <typename EventType>
static inline void HandleObject(const std::string& instance,
                                const KeySetter<EventType>* key_setters,
                                ProtocolSocket& ps)
{
  int key;
  char* key_str;
  const char* value_str;
  char* tmp;
  EventType* event;

  event = new EventType;
  event->SetNagiosInstance(instance);
  key_str = ps.GetLine();
  tmp = strchr(key_str, '=');
  if (!tmp)
    value_str = "";
  else
    {
      *tmp = '\0';
      value_str = tmp + 1;
    }
  key = strtol(key_str, NULL, 0);
  while (key != NDO_API_ENDDATA)
    {
      for (unsigned int i = 0; key_setters[i].key; i++)
        if (key == key_setters[i].key)
          {
            switch (key_setters[i].type)
              {
               case 'b':
                (event->*key_setters[i].setter.set_bool)(strtol(value_str,
                                                                NULL,
                                                                0));
                break ;
               case 'd':
                (event->*key_setters[i].setter.set_double)(strtod(value_str,
                                                                  NULL));
                break ;
               case 'i':
                (event->*key_setters[i].setter.set_int)(strtol(value_str,
                                                               NULL,
                                                               0));
                break ;
               case 's':
                (event->*key_setters[i].setter.set_short)(strtol(value_str,
                                                                 NULL,
                                                                 0));
                break ;
               case 'S':
                (event->*key_setters[i].setter.set_string)(value_str);
                break ;
               case 't':
                (event->*key_setters[i].setter.set_timet)(strtol(value_str,
                                                                 NULL,
                                                                 0));
                break ;
               default:
                logging.LogError("Error while parsing protocol.");
                assert(false);
	      }
	    break ;
	  }
      key_str = ps.GetLine();
      tmp = strchr(key_str, '=');
      if (!tmp)
	value_str = "";
      else
	{
	  *tmp = '\0';
	  value_str = tmp + 1;
	}
      key = strtol(key_str, NULL, 0);
    }
  EventPublisher::GetInstance()->Publish(event);
  return ;
}

/**
 *  NetworkInput constructor.
 */
NetworkInput::NetworkInput(boost::asio::ip::tcp::socket* socket)
  : socket_(new StandardProtocolSocket(socket))
{
#ifndef NDEBUG
  logging.LogDebug("New connection accepted, launching client thread...");
#endif /* !NDEBUG */
  this->thread_ = new boost::thread(boost::ref(*this));
  this->thread_->detach();
  delete (this->thread_);
  this->thread_ = NULL;
}

#ifdef USE_TLS
/**
 *  NetworkInput constructor.
 */
NetworkInput::NetworkInput(
  boost::asio::ssl::stream<boost::asio::ip::tcp::socket>* socket)
  : socket_(new TlsProtocolSocket(socket)), thread_(NULL)
{
#ifndef NDEBUG
  logging.LogDebug("Launching asynchronous TLS handshake...");
#endif /* !NDEBUG */
  socket->async_handshake(boost::asio::ssl::stream_base::server,
			  boost::bind(&NetworkInput::Handshake,
				      this,
				      boost::asio::placeholders::error));
  this->thread_ = NULL;
}
#endif /* USE_TLS */

/**
 *  NetworkInput copy constructor.
 */
NetworkInput::NetworkInput(const NetworkInput& ni)
{
  (void)ni;
}

/**
 *  NetworkInput operator= overload.
 */
NetworkInput& NetworkInput::operator=(const NetworkInput& ni)
{
  (void)ni;
  return (*this);
}

/**
 *  Handle an acknowledgement and publish it against the EventPublisher.
 */
void NetworkInput::HandleAcknowledgement(ProtocolSocket& socket)
{
  static const KeySetter<Acknowledgement> acknowledgement_setters[] =
    {
      { NDO_DATA_ACKNOWLEDGEMENTTYPE,
	's',
	&Acknowledgement::SetAcknowledgementType },
      { NDO_DATA_AUTHORNAME, 'S', &Acknowledgement::SetAuthorName },
      { NDO_DATA_COMMENT, 'S', &Acknowledgement::SetComment },
      { NDO_DATA_HOST, 'S', &Acknowledgement::SetHost },
      { NDO_DATA_NOTIFYCONTACTS, 's', &Acknowledgement::SetNotifyContacts },
      { NDO_DATA_PERSISTENT, 's', &Acknowledgement::SetPersistentComment },
      { NDO_DATA_SERVICE, 'S', &Acknowledgement::SetService },
      { NDO_DATA_STATE, 's', &Acknowledgement::SetState },
      { NDO_DATA_STICKY, 's', &Acknowledgement::SetIsSticky },
      { NDO_DATA_TIMESTAMP, 't', &Acknowledgement::SetEntryTime },
      { 0, '\0', static_cast<void (Acknowledgement::*)(double)>(NULL) }
    };

  HandleObject<Acknowledgement>(this->instance_,
				acknowledgement_setters,
				socket);
  return ;
}

/**
 *  Handle a comment and publish it against the EventPublisher.
 */
void NetworkInput::HandleComment(ProtocolSocket& socket)
{
  static const KeySetter<Comment> comment_setters[] =
    {
      { NDO_DATA_AUTHORNAME, 'S', &Comment::SetAuthorName },
      { NDO_DATA_COMMENT, 'S', &Comment::SetCommentData },
      { NDO_DATA_COMMENTID, 'i', &Comment::SetInternalCommentId },
      { NDO_DATA_COMMENTTYPE, 's', &Comment::SetCommentType },
      { NDO_DATA_ENTRYTIME, 't', &Comment::SetEntryTime },
      { NDO_DATA_ENTRYTYPE, 's', &Comment::SetEntryType },
      { NDO_DATA_EXPIRATIONTIME, 't', &Comment::SetExpireTime },
      { NDO_DATA_EXPIRES, 'b', &Comment::SetExpires },
      { NDO_DATA_HOST, 'S', &Comment::SetHostName },
      { NDO_DATA_PERSISTENT, 'b', &Comment::SetPersistent },
      { NDO_DATA_SERVICE, 'S', &Comment::SetServiceDescription },
      { NDO_DATA_SOURCE, 's', &Comment::SetSource },
      { 0, '\0', static_cast<void (Comment::*)(bool)>(NULL) }
    };

  HandleObject<Comment>(this->instance_, comment_setters, socket);
  return ;
}

/**
 *  Handle a downtime event and publish it against the EventPublisher.
 */
void NetworkInput::HandleDowntime(ProtocolSocket& socket)
{
  static const KeySetter<Downtime> downtime_setters[] =
    {
      { NDO_DATA_AUTHORNAME, 'S', &Downtime::SetAuthorName },
      { NDO_DATA_COMMENT, 'S', &Downtime::SetCommentData },
      { NDO_DATA_DOWNTIMEID, 'i', &Downtime::SetDowntimeId },
      { NDO_DATA_DOWNTIMETYPE, 's', &Downtime::SetDowntimeType },
      { NDO_DATA_DURATION, 's', &Downtime::SetDuration },
      { NDO_DATA_ENDTIME, 't', &Downtime::SetEndTime },
      { NDO_DATA_ENTRYTIME, 't', &Downtime::SetEntryTime },
      { NDO_DATA_FIXED, 'b', &Downtime::SetFixed },
      { NDO_DATA_HOST, 'S', &Downtime::SetHost },
      { NDO_DATA_SERVICE, 'S', &Downtime::SetService },
      { NDO_DATA_STARTTIME, 't', &Downtime::SetStartTime },
      { NDO_DATA_TRIGGEREDBY, 'i', &Downtime::SetTriggeredBy },
      { 0, '\0', static_cast<void (Downtime::*)(double)>(NULL) }
    };

  HandleObject<Downtime>(this->instance_, downtime_setters, socket);
  return ;
}

/**
 *  Handle a host definition event and publish it against the EventPublisher.
 */
void NetworkInput::HandleHost(ProtocolSocket& socket)
{
  static const KeySetter<Host> keys_setters[] =
    {
      { NDO_DATA_ACTIONURL, 'S', &Host::SetActionUrl },
      { NDO_DATA_ACTIVEHOSTCHECKSENABLED, 'b', &Host::SetActiveChecksEnabled },
      { NDO_DATA_DISPLAYNAME, 'S', &Host::SetDisplayName },
      { NDO_DATA_FIRSTNOTIFICATIONDELAY,
        'd',
        &Host::SetFirstNotificationDelay },
      { NDO_DATA_FLAPDETECTIONONDOWN, 's', &Host::SetFlapDetectionOnDown },
      { NDO_DATA_FLAPDETECTIONONUNREACHABLE,
        's',
        &Host::SetFlapDetectionOnUnreachable },
      { NDO_DATA_FLAPDETECTIONONUP, 's', &Host::SetFlapDetectionOnUp },
      { NDO_DATA_HAVE2DCOORDS, 's', &Host::SetHave2DCoords },
      //{ NDO_DATA_HAVE3DCOORDS }
      { NDO_DATA_HIGHHOSTFLAPTHRESHOLD, 'd', &Host::SetHighFlapThreshold },
      { NDO_DATA_HOSTADDRESS, 'S', &Host::SetAddress },
      { NDO_DATA_HOSTALIAS, 'S', &Host::SetAlias },
      { NDO_DATA_HOSTCHECKCOMMAND, 'S', &Host::SetCheckCommand },
      { NDO_DATA_HOSTCHECKINTERVAL, 'd', &Host::SetCheckInterval },
      { NDO_DATA_HOSTCHECKPERIOD, 'S', &Host::SetCheckPeriod },
      { NDO_DATA_HOSTEVENTHANDLER, 'S', &Host::SetEventHandler },
      { NDO_DATA_HOSTEVENTHANDLERENABLED,
        'b',
	&Host::SetEventHandlerEnabled },
      { NDO_DATA_HOSTFAILUREPREDICTIONENABLED,
	'b',
	&Host::SetFailurePredictionEnabled },
      //{ NDO_DATA_HOSTFAILUREPREDICTIONOPTIONS },
      { NDO_DATA_HOSTFLAPDETECTIONENABLED,
        'b',
        &Host::SetFlapDetectionEnabled },
      { NDO_DATA_HOSTFRESHNESSCHECKSENABLED, 'b', &Host::SetCheckFreshness },
      { NDO_DATA_HOSTFRESHNESSTHRESHOLD, 'd', &Host::SetFreshnessThreshold },
      { NDO_DATA_HOSTMAXCHECKATTEMPTS, 's', &Host::SetMaxCheckAttempts },
      { NDO_DATA_HOSTNAME, 'S', &Host::SetHostName },
      { NDO_DATA_HOSTNOTIFICATIONINTERVAL,
        'd',
        &Host::SetNotificationInterval },
      { NDO_DATA_HOSTNOTIFICATIONPERIOD, 'S', &Host::SetNotificationPeriod },
      { NDO_DATA_HOSTNOTIFICATIONSENABLED,
        'b',
	&Host::SetNotificationsEnabled },
      { NDO_DATA_HOSTRETRYINTERVAL, 'd', &Host::SetRetryInterval },
      { NDO_DATA_ICONIMAGE, 'S', &Host::SetIconImage },
      { NDO_DATA_ICONIMAGEALT, 'S', &Host::SetIconImageAlt },
      { NDO_DATA_LOWHOSTFLAPTHRESHOLD, 'd', &Host::SetLowFlapThreshold },
      { NDO_DATA_NOTES, 'S', &Host::SetNotes },
      { NDO_DATA_NOTESURL, 'S', &Host::SetNotesUrl },
      { NDO_DATA_NOTIFYHOSTDOWN, 's', &Host::SetNotifyOnDown },
      { NDO_DATA_NOTIFYHOSTDOWNTIME, 's', &Host::SetNotifyOnDowntime },
      { NDO_DATA_NOTIFYHOSTFLAPPING, 's', &Host::SetNotifyOnFlapping },
      { NDO_DATA_NOTIFYHOSTRECOVERY, 's', &Host::SetNotifyOnRecovery },
      { NDO_DATA_NOTIFYHOSTUNREACHABLE, 's', &Host::SetNotifyOnUnreachable },
      { NDO_DATA_OBSESSOVERHOST, 'b', &Host::SetObsessOver },
      { NDO_DATA_PASSIVEHOSTCHECKSENABLED,
        'b',
        &Host::SetPassiveChecksEnabled },
      { NDO_DATA_PROCESSHOSTPERFORMANCEDATA,
        'b',
        &Host::SetProcessPerformanceData },
      { NDO_DATA_RETAINHOSTNONSTATUSINFORMATION,
	'b',
	&Host::SetRetainNonstatusInformation },
      { NDO_DATA_RETAINHOSTSTATUSINFORMATION,
	'b',
	&Host::SetRetainStatusInformation },
      { NDO_DATA_STALKHOSTONDOWN, 's', &Host::SetStalkOnDown },
      { NDO_DATA_STALKHOSTONUNREACHABLE, 's', &Host::SetStalkOnUnreachable },
      { NDO_DATA_STALKHOSTONUP, 's', &Host::SetStalkOnUp },
      { NDO_DATA_STATUSMAPIMAGE, 'S', &Host::SetStatusmapImage },
      { NDO_DATA_VRMLIMAGE, 'S', &Host::SetVrmlImage },
      { NDO_DATA_X2D, 's', &Host::SetX2D },
      //{ NDO_DATA_X3D },
      { NDO_DATA_Y2D, 's', &Host::SetY2D },
      //{ NDO_DATA_Y3D },
      //{ NDO_DATA_Z3D },
      { 0, '\0', static_cast<void (Host::*)(double)>(NULL) }
    };

  HandleObject<Host>(this->instance_, keys_setters, socket);
  return ;
}

/**
 *  Handle a host group definition and publish it against the EventPublisher.
 */
void NetworkInput::HandleHostGroup(ProtocolSocket& socket)
{
  static const KeySetter<HostGroup> keys_setters[] =
    {
      { NDO_DATA_HOSTGROUPALIAS, 'S', &HostGroup::SetAlias },
      { NDO_DATA_HOSTGROUPNAME, 'S', &HostGroup::SetHostGroupName },
      { 0, '\0', static_cast<void (HostGroup::*)(double)>(NULL) }
    };

  HandleObject<HostGroup>(this->instance_, keys_setters, socket);
  return ;
}

/**
 *  Handle a host status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleHostStatus(ProtocolSocket& socket)
{
  static const KeySetter<HostStatus> keys_setters[] =
      {
	{ NDO_DATA_ACKNOWLEDGEMENTTYPE,
          's',
          &HostStatus::SetAcknowledgementType },
	{ NDO_DATA_ACTIVEHOSTCHECKSENABLED,
          'b',
          &HostStatus::SetActiveChecksEnabled },
	{ NDO_DATA_CHECKCOMMAND, 'S', &HostStatus::SetCheckCommand },
	{ NDO_DATA_CHECKTYPE, 's', &HostStatus::SetCheckType },
	{ NDO_DATA_CURRENTCHECKATTEMPT,
          's',
          &HostStatus::SetCurrentCheckAttempt },
	{ NDO_DATA_CURRENTNOTIFICATIONNUMBER,
	  's',
	  &HostStatus::SetCurrentNotificationNumber },
	{ NDO_DATA_CURRENTSTATE, 's', &HostStatus::SetCurrentState },
	{ NDO_DATA_EVENTHANDLER, 'S', &HostStatus::SetEventHandler },
	{ NDO_DATA_EVENTHANDLERENABLED,
          'b',
          &HostStatus::SetEventHandlerEnabled },
	{ NDO_DATA_EXECUTIONTIME, 'd', &HostStatus::SetExecutionTime },
	{ NDO_DATA_FAILUREPREDICTIONENABLED,
	  'b',
	  &HostStatus::SetFailurePredictionEnabled },
	{ NDO_DATA_FLAPDETECTIONENABLED,
          'b',
          &HostStatus::SetFlapDetectionEnabled },
	{ NDO_DATA_HASBEENCHECKED, 'b', &HostStatus::SetHasBeenChecked },
	{ NDO_DATA_HOST, 'S', &HostStatus::SetHostName },
	{ NDO_DATA_HOSTCHECKPERIOD, 'S', &HostStatus::SetCheckPeriod },
	{ NDO_DATA_ISFLAPPING, 'b', &HostStatus::SetIsFlapping },
	{ NDO_DATA_LASTHARDSTATE, 's', &HostStatus::SetLastHardState },
	{ NDO_DATA_LASTHARDSTATECHANGE,
          't',
          &HostStatus::SetLastHardStateChange },
	{ NDO_DATA_LASTHOSTCHECK, 't', &HostStatus::SetLastCheck },
	{ NDO_DATA_LASTHOSTNOTIFICATION,
          't',
          &HostStatus::SetLastNotification },
	{ NDO_DATA_LASTSTATECHANGE, 't', &HostStatus::SetLastStateChange },
	{ NDO_DATA_LASTTIMEDOWN, 't', &HostStatus::SetLastTimeDown },
	{ NDO_DATA_LASTTIMEUNREACHABLE,
          't',
          &HostStatus::SetLastTimeUnreachable},
	{ NDO_DATA_LATENCY, 'd', &HostStatus::SetLatency },
	{ NDO_DATA_LONGOUTPUT, 'S', &HostStatus::SetLongOutput },
	{ NDO_DATA_MAXCHECKATTEMPTS, 's', &HostStatus::SetMaxCheckAttempts },
	{ NDO_DATA_MODIFIEDHOSTATTRIBUTES,
          'i',
          &HostStatus::SetModifiedAttributes },
	{ NDO_DATA_NEXTHOSTCHECK, 't', &HostStatus::SetNextCheck },
	{ NDO_DATA_NEXTHOSTNOTIFICATION,
          't',
          &HostStatus::SetNextNotification },
	{ NDO_DATA_NOMORENOTIFICATIONS,
          'b',
          &HostStatus::SetNoMoreNotifications },
	{ NDO_DATA_NORMALCHECKINTERVAL,
	  'd',
	  &HostStatus::SetCheckInterval },
	{ NDO_DATA_NOTIFICATIONSENABLED,
          'b',
          &HostStatus::SetNotificationsEnabled },
	{ NDO_DATA_OBSESSOVERHOST, 'b', &HostStatus::SetObsessOver },
	{ NDO_DATA_OUTPUT, 'S', &HostStatus::SetOutput },
	{ NDO_DATA_PASSIVEHOSTCHECKSENABLED,
	  'b',
	  &HostStatus::SetPassiveChecksEnabled },
	{ NDO_DATA_PERCENTSTATECHANGE,
          'd',
          &HostStatus::SetPercentStateChange },
	{ NDO_DATA_PERFDATA, 'S', &HostStatus::SetPerfData },
	{ NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
          'b',
          &HostStatus::SetProblemHasBeenAcknowledged },
	{ NDO_DATA_PROCESSPERFORMANCEDATA,
	  'b',
	  &HostStatus::SetProcessPerformanceData },
	{ NDO_DATA_RETRYCHECKINTERVAL,
	  'd',
	  &HostStatus::SetRetryInterval },
	{ NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
          's',
          &HostStatus::SetScheduledDowntimeDepth },
	{ NDO_DATA_SHOULDBESCHEDULED, 'b', &HostStatus::SetShouldBeScheduled },
	{ NDO_DATA_STATETYPE, 's', &HostStatus::SetStateType },
	{ 0, '\0', static_cast<void (HostStatus::*)(double)>(NULL) }
      };

  HandleObject<HostStatus>(this->instance_, keys_setters, socket);
  return ;
}

/**
 *  Immediately after connection, handle the first data transmitted.
 */
void NetworkInput::HandleInitialization(ProtocolSocket& ps)
{
  char* key;
  char* tmp;
  const char* value;
  Connection*  conn_info;

  conn_info = new Connection;
  conn_info->SetConnectTime(time(NULL));
  key = ps.GetLine();
  while (strcmp(key, NDO_API_STARTDATADUMP))
    {
      tmp = strchr(key, ':');
      if (!tmp || !tmp[1])
	value = "";
      else
	{
	  *tmp = '\0';
	  value = tmp + 2;
	}
      if (!strcmp(key, NDO_API_INSTANCENAME))
	this->instance_ = value;
      else if (!strcmp(key, NDO_API_AGENT))
	conn_info->SetAgentName(value);
      else if (!strcmp(key, NDO_API_AGENTVERSION))
	conn_info->SetAgentVersion(value);
      else if (!strcmp(key, NDO_API_CONNECTION))
	conn_info->SetConnectSource(value);
      else if (!strcmp(key, NDO_API_CONNECTTYPE))
	conn_info->SetConnectType(value);
      key = ps.GetLine();
    }
  conn_info->SetDataStartTime(time(NULL));
  EventPublisher::GetInstance()->Publish(conn_info);
  return ;
}

/**
 *  Handle a program status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleProgramStatus(ProtocolSocket& ps)
{
  static const KeySetter<ProgramStatus> keys_setters[] =
    {
      { NDO_DATA_ACTIVEHOSTCHECKSENABLED,
	's',
	&ProgramStatus::SetActiveHostChecksEnabled },
      { NDO_DATA_ACTIVESERVICECHECKSENABLED,
	's',
	&ProgramStatus::SetActiveServiceChecksEnabled },
      { NDO_DATA_DAEMONMODE, 's', &ProgramStatus::SetDaemonMode },
      { NDO_DATA_EVENTHANDLERENABLED,
        'b',
        &ProgramStatus::SetEventHandlerEnabled },
      { NDO_DATA_FAILUREPREDICTIONENABLED,
	'b',
	&ProgramStatus::SetFailurePredictionEnabled },
      { NDO_DATA_FLAPDETECTIONENABLED,
	'b',
	&ProgramStatus::SetFlapDetectionEnabled },
      { NDO_DATA_GLOBALHOSTEVENTHANDLER,
	'S',
	&ProgramStatus::SetGlobalHostEventHandler },
      { NDO_DATA_GLOBALSERVICEEVENTHANDLER,
	'S',
	&ProgramStatus::SetGlobalServiceEventHandler },
      { NDO_DATA_LASTCOMMANDCHECK,
        't',
        &ProgramStatus::SetLastCommandCheck },
      { NDO_DATA_LASTLOGROTATION,
	't',
	&ProgramStatus::SetLastLogRotation },
      { NDO_DATA_MODIFIEDHOSTATTRIBUTES,
	'i',
	&ProgramStatus::SetModifiedHostAttributes },
      { NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
	'i',
	&ProgramStatus::SetModifiedServiceAttributes },
      { NDO_DATA_NOTIFICATIONSENABLED,
        'b',
        &ProgramStatus::SetNotificationsEnabled },
      { NDO_DATA_OBSESSOVERHOST, 's', &ProgramStatus::SetObsessOverHosts },
      { NDO_DATA_OBSESSOVERSERVICE,
        's',
        &ProgramStatus::SetObsessOverServices },
      { NDO_DATA_PASSIVEHOSTCHECKSENABLED,
	's',
	&ProgramStatus::SetPassiveHostChecksEnabled },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	's',
	&ProgramStatus::SetPassiveServiceChecksEnabled },
      { NDO_DATA_PROCESSPERFORMANCEDATA,
	'b',
	&ProgramStatus::SetProcessPerformanceData },
      { NDO_DATA_PROCESSID, 'i', &ProgramStatus::SetPid },
      { NDO_DATA_PROGRAMSTARTTIME,
	't',
	&ProgramStatus::SetProgramStart },
      { 0, '\0', static_cast<void (ProgramStatus::*)(double)>(NULL) }
    };

  HandleObject<ProgramStatus>(this->instance_, keys_setters, ps);
  return ;
}

/**
 *  Handle a service definition event and publish it against the
 *  EventPublisher.
 */
void NetworkInput::HandleService(ProtocolSocket& ps)
{
  static const KeySetter<Service> keys_setters[] =
    {
      { NDO_DATA_ACTIONURL, 'S', &Service::SetActionUrl },
      { NDO_DATA_ACTIVESERVICECHECKSENABLED,
        'b',
        &Service::SetActiveChecksEnabled },
      { NDO_DATA_DISPLAYNAME, 'S', &Service::SetDisplayName },
      { NDO_DATA_FIRSTNOTIFICATIONDELAY,
        'd',
        &Service::SetFirstNotificationDelay },
      { NDO_DATA_FLAPDETECTIONONCRITICAL,
        's',
        &Service::SetFlapDetectionOnCritical },
      { NDO_DATA_FLAPDETECTIONONOK,
        's',
	&Service::SetFlapDetectionOnOk },
      { NDO_DATA_FLAPDETECTIONONUNKNOWN,
	's',
	&Service::SetFlapDetectionOnUnknown },
      { NDO_DATA_FLAPDETECTIONONWARNING,
	's',
	&Service::SetFlapDetectionOnWarning },
      { NDO_DATA_HIGHSERVICEFLAPTHRESHOLD,
	'd',
	&Service::SetHighFlapThreshold },
      { NDO_DATA_HOSTNAME,
	'S',
	&Service::SetHostName },
      { NDO_DATA_ICONIMAGE,
	'S',
	&Service::SetIconImage },
      { NDO_DATA_ICONIMAGEALT,
	'S',
	&Service::SetIconImageAlt },
      { NDO_DATA_LOWSERVICEFLAPTHRESHOLD,
	'd',
	&Service::SetLowFlapThreshold },
      { NDO_DATA_MAXSERVICECHECKATTEMPTS,
	's',
	&Service::SetMaxCheckAttempts },
      { NDO_DATA_NOTES,
	'S',
	&Service::SetNotes },
      { NDO_DATA_NOTESURL,
	'S',
	&Service::SetNotesUrl },
      { NDO_DATA_NOTIFYSERVICECRITICAL,
	's',
	&Service::SetNotifyOnCritical },
      { NDO_DATA_NOTIFYSERVICEDOWNTIME,
	's',
	&Service::SetNotifyOnDowntime },
      { NDO_DATA_NOTIFYSERVICEFLAPPING,
	's',
	&Service::SetNotifyOnFlapping },
      { NDO_DATA_NOTIFYSERVICERECOVERY,
	's',
	&Service::SetNotifyOnRecovery },
      { NDO_DATA_NOTIFYSERVICEUNKNOWN,
	's',
	&Service::SetNotifyOnUnknown },
      { NDO_DATA_NOTIFYSERVICEWARNING,
	's',
	&Service::SetNotifyOnWarning },
      { NDO_DATA_OBSESSOVERSERVICE,
	'b',
	&Service::SetObsessOver },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	'b',
	&Service::SetPassiveChecksEnabled },
      { NDO_DATA_PROCESSSERVICEPERFORMANCEDATA,
	'b',
	&Service::SetProcessPerformanceData },
      { NDO_DATA_RETAINSERVICENONSTATUSINFORMATION,
	'b',
	&Service::SetRetainNonstatusInformation },
      { NDO_DATA_RETAINSERVICESTATUSINFORMATION,
	'b',
	&Service::SetRetainStatusInformation },
      { NDO_DATA_SERVICECHECKCOMMAND,
	'S',
	&Service::SetCheckCommand },
      { NDO_DATA_SERVICECHECKINTERVAL,
	'd',
	&Service::SetCheckInterval },
      { NDO_DATA_SERVICECHECKPERIOD, 'S', &Service::SetCheckPeriod },
      { NDO_DATA_SERVICEDESCRIPTION,
	'S',
	&Service::SetServiceDescription },
      { NDO_DATA_SERVICEEVENTHANDLER,
	'S',
	&Service::SetEventHandler },
      { NDO_DATA_SERVICEEVENTHANDLERENABLED,
	'b',
	&Service::SetEventHandlerEnabled },
      { NDO_DATA_SERVICEFAILUREPREDICTIONENABLED,
	'b',
	&Service::SetFailurePredictionEnabled },
      { NDO_DATA_SERVICEFAILUREPREDICTIONOPTIONS,
	'S',
	&Service::SetFailurePredictionOptions },
      { NDO_DATA_SERVICEFLAPDETECTIONENABLED,
	'b',
	&Service::SetFlapDetectionEnabled },
      { NDO_DATA_SERVICEFRESHNESSCHECKSENABLED,
	'b',
	&Service::SetCheckFreshness },
      { NDO_DATA_SERVICEFRESHNESSTHRESHOLD,
	'd',
	&Service::SetFreshnessThreshold },
      { NDO_DATA_SERVICEISVOLATILE,
	'b',
	&Service::SetIsVolatile },
      { NDO_DATA_SERVICENOTIFICATIONINTERVAL,
	'd',
	&Service::SetNotificationInterval },
      { NDO_DATA_SERVICENOTIFICATIONPERIOD,
	'S',
	&Service::SetNotificationPeriod },
      { NDO_DATA_SERVICENOTIFICATIONSENABLED,
	'b',
	&Service::SetNotificationsEnabled },
      { NDO_DATA_SERVICERETRYINTERVAL,
	'd',
	&Service::SetRetryInterval },
      { NDO_DATA_STALKSERVICEONCRITICAL,
	's',
	&Service::SetStalkOnCritical },
      { NDO_DATA_STALKSERVICEONOK,
	's',
	&Service::SetStalkOnOk },
      { NDO_DATA_STALKSERVICEONUNKNOWN,
	's',
	&Service::SetStalkOnUnknown },
      { NDO_DATA_STALKSERVICEONWARNING,
	's',
	&Service::SetStalkOnWarning },
      { 0, '\0', static_cast<void (Service::*)(double)>(NULL) }
    };

  HandleObject<Service>(this->instance_, keys_setters, ps);
  return ;
}

/**
 *  Handle a service status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleServiceStatus(ProtocolSocket& ps)
{
  static const KeySetter<ServiceStatus> keys_setters[] =
    {
      { NDO_DATA_ACKNOWLEDGEMENTTYPE,
	's',
	&ServiceStatus::SetAcknowledgementType },
      { NDO_DATA_ACTIVESERVICECHECKSENABLED,
	'b',
	&ServiceStatus::SetActiveChecksEnabled },
      { NDO_DATA_CHECKCOMMAND, 'S', &ServiceStatus::SetCheckCommand },
      { NDO_DATA_CHECKTYPE, 's', &ServiceStatus::SetCheckType },
      { NDO_DATA_CURRENTCHECKATTEMPT,
        's',
        &ServiceStatus::SetCurrentCheckAttempt },
      { NDO_DATA_CURRENTNOTIFICATIONNUMBER,
	's',
	&ServiceStatus::SetCurrentNotificationNumber },
      { NDO_DATA_CURRENTSTATE, 's', &ServiceStatus::SetCurrentState },
      { NDO_DATA_EVENTHANDLER, 'S', &ServiceStatus::SetEventHandler },
      { NDO_DATA_EVENTHANDLERENABLED,
        'b',
        &ServiceStatus::SetEventHandlerEnabled },
      { NDO_DATA_EXECUTIONTIME, 'd', &ServiceStatus::SetExecutionTime },
      { NDO_DATA_FAILUREPREDICTIONENABLED,
	'b',
	&ServiceStatus::SetFailurePredictionEnabled },
      { NDO_DATA_FLAPDETECTIONENABLED,
	'b',
	&ServiceStatus::SetFlapDetectionEnabled },
      { NDO_DATA_HASBEENCHECKED, 'b', &ServiceStatus::SetHasBeenChecked },
      { NDO_DATA_HOST, 'S', &ServiceStatus::SetHostName },
      { NDO_DATA_ISFLAPPING, 'b', &ServiceStatus::SetIsFlapping },
      { NDO_DATA_LASTSERVICECHECK, 't', &ServiceStatus::SetLastCheck },
      { NDO_DATA_LASTHARDSTATE, 's', &ServiceStatus::SetLastHardState },
      { NDO_DATA_LASTHARDSTATECHANGE,
	't',
	&ServiceStatus::SetLastHardStateChange },
      { NDO_DATA_LASTSERVICENOTIFICATION,
        't',
        &ServiceStatus::SetLastNotification },
      { NDO_DATA_LASTSTATECHANGE, 't', &ServiceStatus::SetLastStateChange },
      { NDO_DATA_LASTTIMECRITICAL, 't', &ServiceStatus::SetLastTimeCritical },
      { NDO_DATA_LASTTIMEOK, 't', &ServiceStatus::SetLastTimeOk },
      { NDO_DATA_LASTTIMEUNKNOWN, 't', &ServiceStatus::SetLastTimeUnknown },
      { NDO_DATA_LASTTIMEWARNING, 't', &ServiceStatus::SetLastTimeWarning },
      { NDO_DATA_LATENCY, 'd', &ServiceStatus::SetLatency },
      { NDO_DATA_LONGOUTPUT, 'S', &ServiceStatus::SetLongOutput },
      { NDO_DATA_MAXCHECKATTEMPTS,
	's',
	&ServiceStatus::SetMaxCheckAttempts },
      { NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
	'i',
	&ServiceStatus::SetModifiedAttributes },
      { NDO_DATA_NEXTSERVICECHECK, 't', &ServiceStatus::SetNextCheck },
      { NDO_DATA_NEXTSERVICENOTIFICATION,
	't',
	&ServiceStatus::SetNextNotification },
      { NDO_DATA_NOMORENOTIFICATIONS,
	'b',
	&ServiceStatus::SetNoMoreNotifications },
      { NDO_DATA_NORMALCHECKINTERVAL, 'd', &ServiceStatus::SetCheckInterval },
      { NDO_DATA_OBSESSOVERSERVICE, 'b', &ServiceStatus::SetObsessOver },
      { NDO_DATA_OUTPUT, 'S', &ServiceStatus::SetOutput },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	'b',
	&ServiceStatus::SetPassiveChecksEnabled },
      { NDO_DATA_PERCENTSTATECHANGE,
        'd',
        &ServiceStatus::SetPercentStateChange },
      { NDO_DATA_PERFDATA, 'S', &ServiceStatus::SetPerfData },
      { NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
	'b',
	&ServiceStatus::SetProblemHasBeenAcknowledged },
      { NDO_DATA_PROCESSPERFORMANCEDATA,
	'b',
	&ServiceStatus::SetProcessPerformanceData },
      { NDO_DATA_RETRYCHECKINTERVAL, 'd', &ServiceStatus::SetRetryInterval },
      { NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
	's',
	&ServiceStatus::SetScheduledDowntimeDepth },
      { NDO_DATA_SERVICE, 'S', &ServiceStatus::SetServiceDescription },
      { NDO_DATA_SERVICECHECKPERIOD, 'S', &ServiceStatus::SetCheckPeriod },
      { NDO_DATA_SHOULDBESCHEDULED,
        'b',
        &ServiceStatus::SetShouldBeScheduled },
      { NDO_DATA_STATETYPE, 's', &ServiceStatus::SetStateType },
      { 0, '\0', static_cast<void (ServiceStatus::*)(double)>(NULL) }
    };

  HandleObject<ServiceStatus>(this->instance_, keys_setters, ps);
  return ;
}

#ifdef USE_TLS
/**
 *  Handle the TLS handshake.
 */
void NetworkInput::Handshake(const boost::system::error_code& ec)
{
  if (!ec)
    {
# ifndef NDEBUG
      logging.LogDebug("TLS handshake succeeded, launching client thread...");
# endif /* !NDEBUG */
      this->thread_ = new boost::thread(boost::ref(*this));
      this->thread_->detach();
      delete (this->thread_);
      this->thread_ = NULL;
    }
  else
    {
      logging.LogInfo("TLS handshake failed, closing connection...", true);
      logging.LogInfo(ec.message().c_str());
      logging.Deindent();
      delete (this);
    }
  return ;
}
#endif /* USE_TLS */

/**************************************
*                                     *
*            Public Methods           *
*                                     *
**************************************/

/**
 *  NetworkInput destructor.
 */
NetworkInput::~NetworkInput() throw ()
{
  logging.LogInfo("Closing client connection...");
  // We might end up here and the object already been destroyed so let's be
  // extra careful.
  try
    {
      boost::unique_lock<boost::mutex> lock(gl_mutex);
      std::list<NetworkInput*>::iterator it;

      // XXX : use std::algorithm
      for (it = gl_ni.begin(); it != gl_ni.end(); it++)
        if (*it == this)
          {
            gl_ni.erase(it);
            break ;
          }
    }
  catch (...)
    {
    }
}

/**
 *  Thread entry point.
 */
void NetworkInput::operator()()
{
  char* buffer;
  static const struct
  {
    int event;
    void (NetworkInput::* handler)(ProtocolSocket&);
  } handlers[] =
      {
	{ NDO_API_ACKNOWLEDGEMENTDATA, &NetworkInput::HandleAcknowledgement },
	{ NDO_API_COMMENTDATA, &NetworkInput::HandleComment },
	{ NDO_API_HOSTDEFINITION, &NetworkInput::HandleHost },
	{ NDO_API_HOSTGROUPDEFINITION, &NetworkInput::HandleHostGroup },
	{ NDO_API_HOSTSTATUSDATA, &NetworkInput::HandleHostStatus },
	{ NDO_API_PROGRAMSTATUSDATA, &NetworkInput::HandleProgramStatus },
	{ NDO_API_SERVICEDEFINITION, &NetworkInput::HandleService },
	{ NDO_API_SERVICESTATUSDATA, &NetworkInput::HandleServiceStatus },
	{ 0, NULL }
      };

  try
    {
      HandleInitialization(*this->socket_);
      buffer = this->socket_->GetLine();
      while (strcmp(buffer, NDO_API_GOODBYE))
	{
	  if (4 == strlen(buffer) && ':' == buffer[3])
	    {
	      int event;

	      buffer[3] = '\0';
	      event = strtol(buffer, NULL, 0);
	      for (unsigned int i = 0; handlers[i].event; i++)
		if (handlers[i].event == event)
		  {
		    this->conn_status_.SetBytesProcessed(
		      this->socket_->GetBytesProcessed());
		    this->conn_status_.SetLinesProcessed(
                      this->socket_->GetLinesProcessed());
		    (this->*(handlers[i].handler))(*this->socket_);
		    this->conn_status_.SetEntriesProcessed(
                      this->conn_status_.GetEntriesProcessed() + 1);
		    this->conn_status_.SetLastCheckinTime(
                      this->socket_->GetLastCheckinTime());
		    EventPublisher::GetInstance()->Publish(
		      new ConnectionStatus(this->conn_status_));
		    break ;
		  }
	    }
	  buffer = this->socket_->GetLine();
	}
      this->conn_status_.SetDataEndTime(time(NULL));
    }
  catch (std::exception& e)
    {
      logging.LogError("Exception occured while processing network input",
		       true);
      logging.LogError(e.what());
      logging.Deindent();
    }
  catch (...)
    {
      logging.LogError(
        "Unknown exception occured while processing network input");
    }
  this->conn_status_.SetDisconnectTime(time(NULL));
  try
    {
      EventPublisher::GetInstance()->Publish(new ConnectionStatus(
		                               this->conn_status_));
    }
  catch (...)
    {
    }
  delete (this);
  return ;
}
