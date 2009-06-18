/*
** network_input.cpp for CentreonBroker in ./src
** 
** Made by Matthieu Kermagoret <mkermagoret@merethis.com>
** 
** Copyright Merethis
** See LICENSE file for details.
** 
** Started on  05/11/09 Matthieu Kermagoret
** Last update 06/18/09 Matthieu Kermagoret
*/

#include <boost/thread/mutex.hpp>
#include <cstdlib>
#include <ctime>
#include "acknowledgement.h"
#include "comment.h"
#include "connection.h"
#include "event_publisher.h"
#include "host.h"
#include "host_status.h"
#include "logging.h"
#include "nagios/protoapi.h"
#include "network_input.h"
#include "program_status.h"
#include "service.h"
#include "service_status.h"

using namespace CentreonBroker;

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
    void (Event::* set_double)(double);
    void (Event::* set_int)(int);
    void (Event::* set_short)(short);
    void (Event::* set_string)(const std::string&);
    void (Event::* set_timet)(time_t);

    UHandler() : set_double(NULL) {}
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
                std::cerr << "Wrong type : " << key_setters[i].type
                          << std::endl;
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
  logging.AddDebug("New connection accepted, launching client thread...");
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
  logging.AddDebug("Launching asynchronous TLS handshake...");
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
      //{ NDO_DATA_COMMENTID },
      { NDO_DATA_COMMENTTYPE, 's', &Comment::SetCommentType },
      { NDO_DATA_ENTRYTIME, 't', &Comment::SetEntryTime },
      { NDO_DATA_ENTRYTYPE, 's', &Comment::SetEntryType },
      { NDO_DATA_EXPIRATIONTIME, 't', &Comment::SetExpirationTime },
      { NDO_DATA_EXPIRES, 's', &Comment::SetExpires },
      { NDO_DATA_HOST, 'S', &Comment::SetHost },
      { NDO_DATA_PERSISTENT, 's', &Comment::SetIsPersistent },
      { NDO_DATA_SERVICE, 'S', &Comment::SetService },
      { NDO_DATA_SOURCE, 's', &Comment::SetCommentSource },
      { 0, '\0', static_cast<void (Comment::*)(double)>(NULL) }
    };

  HandleObject<Comment>(this->instance_, comment_setters, socket);
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
      { NDO_DATA_ACTIVEHOSTCHECKSENABLED, 's', &Host::SetActiveChecksEnabled },
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
      //{ NDO_DATA_HOSTCHECKPERIOD },
      { NDO_DATA_HOSTEVENTHANDLER, 'S', &Host::SetEventHandler },
      { NDO_DATA_HOSTEVENTHANDLERENABLED,
        's',
	&Host::SetEventHandlerEnabled },
      { NDO_DATA_HOSTFAILUREPREDICTIONENABLED,
	's',
	&Host::SetFailurePredictionEnabled },
      //{ NDO_DATA_HOSTFAILUREPREDICTIONOPTIONS },
      { NDO_DATA_HOSTFLAPDETECTIONENABLED,
        's',
        &Host::SetFlapDetectionEnabled },
      //{ NDO_DATA_HOSTFRESHNESSCHECKSENABLED },
      { NDO_DATA_HOSTFRESHNESSTHRESHOLD, 's', &Host::SetFreshnessThreshold },
      { NDO_DATA_HOSTMAXCHECKATTEMPTS, 's', &Host::SetMaxCheckAttempts },
      { NDO_DATA_HOSTNAME, 'S', &Host::SetHostName },
      { NDO_DATA_HOSTNOTIFICATIONINTERVAL,
        'd',
        &Host::SetNotificationInterval },
      //{ NDO_DATA_HOSTNOTIFICATIONPERIOD },
      { NDO_DATA_HOSTNOTIFICATIONSENABLED,
        's',
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
      { NDO_DATA_OBSESSOVERHOST, 's', &Host::SetObsessOver },
      { NDO_DATA_PASSIVEHOSTCHECKSENABLED,
        's',
        &Host::SetPassiveChecksEnabled },
      { NDO_DATA_PROCESSHOSTPERFORMANCEDATA,
        's',
        &Host::SetProcessPerformanceData },
      { NDO_DATA_RETAINHOSTNONSTATUSINFORMATION,
	's',
	&Host::SetRetainNonstatusInformation },
      { NDO_DATA_RETAINHOSTSTATUSINFORMATION,
	's',
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
          's',
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
          's',
          &HostStatus::SetEventHandlerEnabled },
	{ NDO_DATA_EXECUTIONTIME, 'd', &HostStatus::SetExecutionTime },
	{ NDO_DATA_FAILUREPREDICTIONENABLED,
	  's',
	  &HostStatus::SetFailurePredictionEnabled },
	{ NDO_DATA_FLAPDETECTIONENABLED,
          's',
          &HostStatus::SetFlapDetectionEnabled },
	{ NDO_DATA_HASBEENCHECKED, 's', &HostStatus::SetHasBeenChecked },
	{ NDO_DATA_HOST, 'S', &HostStatus::SetHostName },
	{ NDO_DATA_ISFLAPPING, 's', &HostStatus::SetIsFlapping },
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
	{ NDO_DATA_MAXCHECKATTEMPTS, 's', &HostStatus::SetMaxCheckAttempts },
	{ NDO_DATA_MODIFIEDHOSTATTRIBUTES,
          'i',
          &HostStatus::SetModifiedAttributes },
	{ NDO_DATA_NEXTHOSTCHECK, 't', &HostStatus::SetNextCheck },
	{ NDO_DATA_NEXTHOSTNOTIFICATION,
          't',
          &HostStatus::SetNextNotification },
	{ NDO_DATA_NOMORENOTIFICATIONS,
          's',
          &HostStatus::SetNoMoreNotifications },
	{ NDO_DATA_NORMALCHECKINTERVAL,
	  'd',
	  &HostStatus::SetCheckInterval },
	{ NDO_DATA_NOTIFICATIONSENABLED,
          's',
          &HostStatus::SetNotificationsEnabled },
	{ NDO_DATA_OBSESSOVERHOST, 's', &HostStatus::SetObsessOver },
	{ NDO_DATA_OUTPUT, 'S', &HostStatus::SetOutput },
	{ NDO_DATA_PASSIVEHOSTCHECKSENABLED,
	  's',
	  &HostStatus::SetPassiveChecksEnabled },
	{ NDO_DATA_PERCENTSTATECHANGE,
          'd',
          &HostStatus::SetPercentStateChange },
	{ NDO_DATA_PERFDATA, 'S', &HostStatus::SetPerfdata },
	{ NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
          's',
          &HostStatus::SetProblemHasBeenAcknowledged },
	{ NDO_DATA_PROCESSPERFORMANCEDATA,
	  's',
	  &HostStatus::SetProcessPerformanceData },
	{ NDO_DATA_RETRYCHECKINTERVAL,
	  'd',
	  &HostStatus::SetRetryInterval },
	{ NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
          's',
          &HostStatus::SetScheduledDowntimeDepth },
	{ NDO_DATA_SHOULDBESCHEDULED, 's', &HostStatus::SetShouldBeScheduled },
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
        's',
        &ProgramStatus::SetEventHandlerEnabled },
      { NDO_DATA_FAILUREPREDICTIONENABLED,
	's',
	&ProgramStatus::SetFailurePredictionEnabled },
      { NDO_DATA_FLAPDETECTIONENABLED,
	's',
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
        's',
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
	's',
	&ProgramStatus::SetProcessPerformanceData },
      { NDO_DATA_PROCESSID, 'i', &ProgramStatus::SetProcessId },
      { NDO_DATA_PROGRAMSTARTTIME,
	't',
	&ProgramStatus::SetProgramStartTime },
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
        's',
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
	's',
	&Service::SetObsessOver },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	's',
	&Service::SetPassiveChecksEnabled },
      { NDO_DATA_PROCESSSERVICEPERFORMANCEDATA,
	's',
	&Service::SetProcessPerformanceData },
      { NDO_DATA_RETAINSERVICENONSTATUSINFORMATION,
	's',
	&Service::SetRetainNonstatusInformation },
      { NDO_DATA_RETAINSERVICESTATUSINFORMATION,
	's',
	&Service::SetRetainStatusInformation },
      { NDO_DATA_SERVICECHECKCOMMAND,
	'S',
	&Service::SetCheckCommand },
      { NDO_DATA_SERVICECHECKINTERVAL,
	'd',
	&Service::SetCheckInterval },
      //{ NDO_DATA_SERVICECHECKPERIOD },
      { NDO_DATA_SERVICEDESCRIPTION,
	'S',
	&Service::SetServiceDescription },
      { NDO_DATA_SERVICEEVENTHANDLER,
	'S',
	&Service::SetEventHandler },
      { NDO_DATA_SERVICEEVENTHANDLERENABLED,
	's',
	&Service::SetEventHandlerEnabled },
      { NDO_DATA_SERVICEFAILUREPREDICTIONENABLED,
	's',
	&Service::SetFailurePredictionEnabled },
      { NDO_DATA_SERVICEFAILUREPREDICTIONOPTIONS,
	'S',
	&Service::SetFailurePredictionOptions },
      { NDO_DATA_SERVICEFLAPDETECTIONENABLED,
	's',
	&Service::SetFlapDetectionEnabled },
      { NDO_DATA_SERVICEFRESHNESSCHECKSENABLED,
	's',
	&Service::SetFreshnessChecksEnabled },
      { NDO_DATA_SERVICEFRESHNESSTHRESHOLD,
	's',
	&Service::SetFreshnessThreshold },
      { NDO_DATA_SERVICEISVOLATILE,
	's',
	&Service::SetIsVolatile },
      { NDO_DATA_SERVICENOTIFICATIONINTERVAL,
	'd',
	&Service::SetNotificationInterval },
      //{ NDO_DATA_SERVICENOTIFICATIONPERIOD },
      { NDO_DATA_SERVICENOTIFICATIONSENABLED,
	's',
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
	's',
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
        's',
        &ServiceStatus::SetEventHandlerEnabled },
      { NDO_DATA_EXECUTIONTIME, 'd', &ServiceStatus::SetExecutionTime },
      { NDO_DATA_FAILUREPREDICTIONENABLED,
	's',
	&ServiceStatus::SetFailurePredictionEnabled },
      { NDO_DATA_FLAPDETECTIONENABLED,
	's',
	&ServiceStatus::SetFlapDetectionEnabled },
      { NDO_DATA_HASBEENCHECKED, 's', &ServiceStatus::SetHasBeenChecked },
      { NDO_DATA_HOST, 'S', &ServiceStatus::SetHostName },
      { NDO_DATA_ISFLAPPING, 's', &ServiceStatus::SetIsFlapping },
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
	's',
	&ServiceStatus::SetNoMoreNotifications },
      { NDO_DATA_NORMALCHECKINTERVAL, 'd', &ServiceStatus::SetCheckInterval },
      { NDO_DATA_OBSESSOVERSERVICE, 's', &ServiceStatus::SetObsessOver },
      { NDO_DATA_OUTPUT, 'S', &ServiceStatus::SetOutput },
      { NDO_DATA_PASSIVESERVICECHECKSENABLED,
	's',
	&ServiceStatus::SetPassiveChecksEnabled },
      { NDO_DATA_PERCENTSTATECHANGE,
        'd',
        &ServiceStatus::SetPercentStateChange },
      { NDO_DATA_PERFDATA, 'S', &ServiceStatus::SetPerfdata },
      { NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
	's',
	&ServiceStatus::SetProblemHasBeenAcknowledged },
      { NDO_DATA_PROCESSPERFORMANCEDATA,
	's',
	&ServiceStatus::SetProcessPerformanceData },
      { NDO_DATA_RETRYCHECKINTERVAL, 'd', &ServiceStatus::SetRetryInterval },
      { NDO_DATA_SERVICE, 'S', &ServiceStatus::SetServiceDescription },
      { NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
	's',
	&ServiceStatus::SetScheduledDowntimeDepth },
      { NDO_DATA_SHOULDBESCHEDULED,
        's',
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
      logging.AddDebug("TLS handshake succeeded, launching client thread...");
# endif /* !NDEBUG */
      this->thread_ = new boost::thread(boost::ref(*this));
      this->thread_->detach();
      delete (this->thread_);
      this->thread_ = NULL;
    }
  else
    {
      logging.AddInfo("TLS handshake failed, closing connection...");
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
  logging.AddInfo("Closing client connection...");
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
  catch (...)
    {
      // XXX : some error message, somewhere
    }
  this->conn_status_.SetDisconnectTime(time(NULL));
  EventPublisher::GetInstance()->Publish(new ConnectionStatus(
                                           this->conn_status_));
  delete (this);
  return ;
}
