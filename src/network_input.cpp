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

#include <cstdlib>
#include <cstring>
#include <ctime>
#include "client_acceptor.h"
#include "event_publisher.h"
#include "events/acknowledgement.h"
#include "events/comment.h"
#include "events/downtime.h"
#include "events/host.h"
#include "events/host_group.h"
#include "events/host_status.h"
#include "events/log.h"
#include "events/program_status.h"
#include "events/service.h"
#include "events/service_status.h"
#include "exception.h"
#include "logging.h"
#include "nagios/protoapi.h"
#include "network_input.h"

using namespace CentreonBroker;
using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Append a member to the host group.
 */
static void AddHostGroupMember(HostGroup& host_group, const char* member)
{
  host_group.members.push_back(member);
  return ;
}

/**
 *  Parse a custom var and if matching set the corresponding host_id.
 */
static void CustomVarToHost(Host& host, const char* custom_var)
{
  int skip;

  skip = strcspn(custom_var, ":");
  if (!strncmp(custom_var, "HOST_ID", skip) && (custom_var[skip] == ':'))
    host.host_id = strtol(strrchr(custom_var + skip, ':') + 1, NULL, 0);
  return ;
}

/**
 *  Parse a custom var and if matching set the corresponding service_id.
 */
static void CustomVarToService(Service& service, const char* custom_var)
{
  int skip;

  skip = strcspn(custom_var, ":");
  if (!strncmp(custom_var, "SERVICE_ID", skip) && (custom_var[skip] == ':'))
    service.service_id = strtol(strrchr(custom_var + skip, ':') + 1, NULL, 0);
  return ;
}

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
    bool (Event::* set_bool);
    double (Event::* set_double);
    int (Event::* set_int);
    short (Event::* set_short);
    std::string (Event::* set_string);
    time_t (Event::* set_timet);
    void (* set_undefined)(Event&, const char*);
  } setter;

  KeySetter() : key(0), type('\0')
  { this->setter.set_bool = NULL; }
  KeySetter(int k, bool (Event::* sb)) : key(k), type('b')
  { this->setter.set_bool = sb; }
  KeySetter(int k, double (Event::* sd)) : key(k), type('d')
  { this->setter.set_double = sd; }
  KeySetter(int k, int (Event::* si)) : key(k), type('i')
  { this->setter.set_int = si; }
  KeySetter(int k, short (Event::* ss)) : key(k), type('s')
  { this->setter.set_short = ss; }
  KeySetter(int k, std::string (Event::* ss)) : key(k), type('S')
  { this->setter.set_string = ss; }
  KeySetter(int k, time_t (Event::* st)) : key(k), type('t')
  { this->setter.set_timet = st; }
  KeySetter(int k, void (* su)(Event&, const char*)) : key(k), type('u')
  { this->setter.set_undefined = su; }
};

/**
 *  For all kind of events, this template function will parse the socket input,
 *  fill the object and publish it.
 */
template <typename EventType>
static inline void HandleObject(const std::string& instance,
                                const KeySetter<EventType>* key_setters,
                                NetworkInput& ni)
{
  int key;
  char* key_str;
  const char* value_str;
  char* tmp;
  EventType* event;

  event = new EventType;
  event->instance = instance;
  key_str = ni.GetLine();
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
                event->*key_setters[i].setter.set_bool = strtol(value_str,
                                                                NULL,
                                                                0);
                break ;
               case 'd':
                event->*key_setters[i].setter.set_double = strtod(value_str,
                                                                  NULL);
                break ;
               case 'i':
                event->*key_setters[i].setter.set_int = strtol(value_str,
                                                               NULL,
                                                               0);
                break ;
               case 's':
                event->*key_setters[i].setter.set_short = strtol(value_str,
                                                                 NULL,
                                                                 0);
                break ;
               case 'S':
                event->*key_setters[i].setter.set_string = value_str;
                break ;
               case 't':
                event->*key_setters[i].setter.set_timet = strtol(value_str,
                                                                 NULL,
                                                                 0);
                break ;
               case 'u':
		key_setters[i].setter.set_undefined(*event, value_str);
		break ;
               default:
                logging.LogError("Error while parsing protocol.");
                assert(false);
	      }
	    break ;
	  }
      key_str = ni.GetLine();
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
  EventPublisher::GetInstance().Publish(event);
  return ;
}

/**
 *  NetworkInput constructor.
 */
NetworkInput::NetworkInput(ClientAcceptor* parent, IO::Stream* stream)
  : discard_(0),
    length_(0),
    parent_(parent),
    socket_(stream)
{
#ifndef NDEBUG
  logging.LogDebug("New connection accepted, launching client thread...");
#endif /* !NDEBUG */
  boost::unique_lock<boost::mutex> lock(this->threadm_);

  this->buffer_[0] = '\0';
  this->thread_ = new boost::thread(boost::ref(*this));
}

/**
 *  NetworkInput copy constructor.
 */
NetworkInput::NetworkInput(const NetworkInput& ni)
{
  (void)ni;
  assert(false);
}

/**
 *  NetworkInput operator= overload.
 */
NetworkInput& NetworkInput::operator=(const NetworkInput& ni)
{
  (void)ni;
  assert(false);
  return (*this);
}

/**
 *  Handle an acknowledgement and publish it against the EventPublisher.
 */
void NetworkInput::HandleAcknowledgement()
{
  static const KeySetter<Acknowledgement> acknowledgement_setters[] =
    {
      KeySetter<Acknowledgement>(NDO_DATA_ACKNOWLEDGEMENTTYPE,
        &Acknowledgement::type),
      KeySetter<Acknowledgement>(NDO_DATA_AUTHORNAME,
        &Acknowledgement::author),
      KeySetter<Acknowledgement>(NDO_DATA_COMMENT,
        &Acknowledgement::comment),
      KeySetter<Acknowledgement>(NDO_DATA_HOST,
        &Acknowledgement::host),
      KeySetter<Acknowledgement>(NDO_DATA_NOTIFYCONTACTS,
        &Acknowledgement::notify_contacts),
      KeySetter<Acknowledgement>(NDO_DATA_PERSISTENT,
        &Acknowledgement::persistent_comment),
      KeySetter<Acknowledgement>(NDO_DATA_SERVICE,
        &Acknowledgement::service),
      KeySetter<Acknowledgement>(NDO_DATA_STATE,
        &Acknowledgement::state),
      KeySetter<Acknowledgement>(NDO_DATA_STICKY,
        &Acknowledgement::is_sticky),
      KeySetter<Acknowledgement>(NDO_DATA_TIMESTAMP,
        &Acknowledgement::entry_time),
      KeySetter<Acknowledgement>()
    };

  HandleObject<Acknowledgement>(this->instance_,
				acknowledgement_setters,
				*this);
  return ;
}

/**
 *  Handle a comment and publish it against the EventPublisher.
 */
void NetworkInput::HandleComment()
{
  static const KeySetter<Comment> comment_setters[] =
    {
      KeySetter<Comment>(NDO_DATA_AUTHORNAME,
        &Comment::author),
      KeySetter<Comment>(NDO_DATA_COMMENT,
        &Comment::comment),
      KeySetter<Comment>(NDO_DATA_COMMENTID,
        &Comment::internal_id),
      KeySetter<Comment>(NDO_DATA_COMMENTTYPE,
        &Comment::type),
      KeySetter<Comment>(NDO_DATA_ENTRYTIME,
        &Comment::entry_time),
      KeySetter<Comment>(NDO_DATA_ENTRYTYPE,
        &Comment::entry_type),
      KeySetter<Comment>(NDO_DATA_EXPIRATIONTIME,
        &Comment::expire_time),
      KeySetter<Comment>(NDO_DATA_EXPIRES,
        &Comment::expires),
      KeySetter<Comment>(NDO_DATA_HOST,
        &Comment::host),
      KeySetter<Comment>(NDO_DATA_PERSISTENT,
        &Comment::persistent),
      KeySetter<Comment>(NDO_DATA_SERVICE,
        &Comment::service),
      KeySetter<Comment>(NDO_DATA_SOURCE,
        &Comment::source),
      KeySetter<Comment>()
    };

  HandleObject<Comment>(this->instance_, comment_setters, *this);
  return ;
}

/**
 *  Handle a downtime event and publish it against the EventPublisher.
 */
void NetworkInput::HandleDowntime()
{
  static const KeySetter<Downtime> downtime_setters[] =
    {
      KeySetter<Downtime>(NDO_DATA_AUTHORNAME,
        &Downtime::author),
      KeySetter<Downtime>(NDO_DATA_COMMENT,
        &Downtime::comment),
      KeySetter<Downtime>(NDO_DATA_DOWNTIMEID,
        &Downtime::id),
      KeySetter<Downtime>(NDO_DATA_DOWNTIMETYPE,
        &Downtime::type),
      KeySetter<Downtime>(NDO_DATA_DURATION,
        &Downtime::duration),
      KeySetter<Downtime>(NDO_DATA_ENDTIME,
        &Downtime::end_time),
      KeySetter<Downtime>(NDO_DATA_ENTRYTIME,
        &Downtime::entry_time),
      KeySetter<Downtime>(NDO_DATA_FIXED,
        &Downtime::fixed),
      KeySetter<Downtime>(NDO_DATA_HOST,
        &Downtime::host),
      KeySetter<Downtime>(NDO_DATA_SERVICE,
        &Downtime::service),
      KeySetter<Downtime>(NDO_DATA_STARTTIME,
        &Downtime::start_time),
      KeySetter<Downtime>(NDO_DATA_TRIGGEREDBY,
        &Downtime::triggered_by),
      KeySetter<Downtime>()
    };

  HandleObject<Downtime>(this->instance_, downtime_setters, *this);
  return ;
}

/**
 *  Handle a host definition event and publish it against the EventPublisher.
 */
void NetworkInput::HandleHost()
{
  static const KeySetter<Host> keys_setters[] =
    {
      KeySetter<Host>(NDO_DATA_ACTIONURL,
        &Host::action_url),
      KeySetter<Host>(NDO_DATA_ACTIVEHOSTCHECKSENABLED,
        &Host::active_checks_enabled),
      KeySetter<Host>(NDO_DATA_CUSTOMVARIABLE,
        &CustomVarToHost),
      KeySetter<Host>(NDO_DATA_DISPLAYNAME,
        &Host::display_name),
      KeySetter<Host>(NDO_DATA_FIRSTNOTIFICATIONDELAY,
        &Host::first_notification_delay),
      KeySetter<Host>(NDO_DATA_FLAPDETECTIONONDOWN,
        &Host::flap_detection_on_down),
      KeySetter<Host>(NDO_DATA_FLAPDETECTIONONUNREACHABLE,
        &Host::flap_detection_on_unreachable),
      KeySetter<Host>(NDO_DATA_FLAPDETECTIONONUP,
        &Host::flap_detection_on_up),
      KeySetter<Host>(NDO_DATA_HAVE2DCOORDS,
        &Host::have_2d_coords),
      //{ NDO_DATA_HAVE3DCOORDS }
      KeySetter<Host>(NDO_DATA_HIGHHOSTFLAPTHRESHOLD,
        &Host::high_flap_threshold),
      KeySetter<Host>(NDO_DATA_HOSTADDRESS,
        &Host::address),
      KeySetter<Host>(NDO_DATA_HOSTALIAS,
        &Host::alias),
      KeySetter<Host>(NDO_DATA_HOSTCHECKCOMMAND,
        &Host::check_command),
      KeySetter<Host>(NDO_DATA_HOSTCHECKINTERVAL,
        &Host::check_interval),
      KeySetter<Host>(NDO_DATA_HOSTCHECKPERIOD,
        &Host::check_period),
      KeySetter<Host>(NDO_DATA_HOSTEVENTHANDLER,
        &Host::event_handler),
      KeySetter<Host>(NDO_DATA_HOSTEVENTHANDLERENABLED,
        &Host::event_handler_enabled),
      KeySetter<Host>(NDO_DATA_HOSTFAILUREPREDICTIONENABLED,
        &Host::failure_prediction_enabled),
      //{ NDO_DATA_HOSTFAILUREPREDICTIONOPTIONS },
      KeySetter<Host>(NDO_DATA_HOSTFLAPDETECTIONENABLED,
        &Host::flap_detection_enabled),
      KeySetter<Host>(NDO_DATA_HOSTFRESHNESSCHECKSENABLED,
        &Host::check_freshness),
      KeySetter<Host>(NDO_DATA_HOSTFRESHNESSTHRESHOLD,
        &Host::freshness_threshold),
      KeySetter<Host>(NDO_DATA_HOSTMAXCHECKATTEMPTS,
        &Host::max_check_attempts),
      KeySetter<Host>(NDO_DATA_HOSTNAME,
        &Host::host),
      KeySetter<Host>(NDO_DATA_HOSTNOTIFICATIONINTERVAL,
        &Host::notification_interval),
      KeySetter<Host>(NDO_DATA_HOSTNOTIFICATIONPERIOD,
        &Host::notification_period),
      KeySetter<Host>(NDO_DATA_HOSTNOTIFICATIONSENABLED,
        &Host::notifications_enabled),
      KeySetter<Host>(NDO_DATA_HOSTRETRYINTERVAL,
        &Host::retry_interval),
      KeySetter<Host>(NDO_DATA_ICONIMAGE,
        &Host::icon_image),
      KeySetter<Host>(NDO_DATA_ICONIMAGEALT,
        &Host::icon_image_alt),
      KeySetter<Host>(NDO_DATA_LOWHOSTFLAPTHRESHOLD,
        &Host::low_flap_threshold),
      KeySetter<Host>(NDO_DATA_NOTES,
        &Host::notes),
      KeySetter<Host>(NDO_DATA_NOTESURL,
        &Host::notes_url),
      KeySetter<Host>(NDO_DATA_NOTIFYHOSTDOWN,
        &Host::notify_on_down),
      KeySetter<Host>(NDO_DATA_NOTIFYHOSTDOWNTIME,
        &Host::notify_on_downtime),
      KeySetter<Host>(NDO_DATA_NOTIFYHOSTFLAPPING,
        &Host::notify_on_flapping),
      KeySetter<Host>(NDO_DATA_NOTIFYHOSTRECOVERY,
        &Host::notify_on_recovery),
      KeySetter<Host>(NDO_DATA_NOTIFYHOSTUNREACHABLE,
        &Host::notify_on_unreachable),
      KeySetter<Host>(NDO_DATA_OBSESSOVERHOST,
        &Host::obsess_over),
      KeySetter<Host>(NDO_DATA_PASSIVEHOSTCHECKSENABLED,
        &Host::passive_checks_enabled),
      KeySetter<Host>(NDO_DATA_PROCESSHOSTPERFORMANCEDATA,
        &Host::process_performance_data),
      KeySetter<Host>(NDO_DATA_RETAINHOSTNONSTATUSINFORMATION,
        &Host::retain_nonstatus_information),
      KeySetter<Host>(NDO_DATA_RETAINHOSTSTATUSINFORMATION,
        &Host::retain_status_information),
      KeySetter<Host>(NDO_DATA_STALKHOSTONDOWN,
        &Host::stalk_on_down),
      KeySetter<Host>(NDO_DATA_STALKHOSTONUNREACHABLE,
        &Host::stalk_on_unreachable),
      KeySetter<Host>(NDO_DATA_STALKHOSTONUP,
        &Host::stalk_on_up),
      KeySetter<Host>(NDO_DATA_STATUSMAPIMAGE,
        &Host::statusmap_image),
      KeySetter<Host>(NDO_DATA_VRMLIMAGE,
        &Host::vrml_image),
      KeySetter<Host>(NDO_DATA_X2D,
        &Host::x_2d),
      //{ NDO_DATA_X3D },
      KeySetter<Host>(NDO_DATA_Y2D,
        &Host::y_2d),
      //{ NDO_DATA_Y3D },
      //{ NDO_DATA_Z3D },
      KeySetter<Host>()
    };

  HandleObject<Host>(this->instance_, keys_setters, *this);
  return ;
}

/**
 *  Handle a host group definition and publish it against the EventPublisher.
 */
void NetworkInput::HandleHostGroup()
{
  static const KeySetter<HostGroup> keys_setters[] =
    {
      KeySetter<HostGroup>(NDO_DATA_HOSTGROUPALIAS,
        &HostGroup::alias),
      KeySetter<HostGroup>(NDO_DATA_HOSTGROUPMEMBER,
        &AddHostGroupMember),
      KeySetter<HostGroup>(NDO_DATA_HOSTGROUPNAME,
        &HostGroup::name),
      KeySetter<HostGroup>()
    };

  HandleObject<HostGroup>(this->instance_, keys_setters, *this);
  return ;
}

/**
 *  Handle a host status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleHostStatus()
{
  static const KeySetter<HostStatus> keys_setters[] =
    {
      KeySetter<HostStatus>(NDO_DATA_ACKNOWLEDGEMENTTYPE,
        &HostStatus::acknowledgement_type),
      KeySetter<HostStatus>(NDO_DATA_ACTIVEHOSTCHECKSENABLED,
        &HostStatus::active_checks_enabled),
      KeySetter<HostStatus>(NDO_DATA_CHECKCOMMAND,
        &HostStatus::check_command),
      KeySetter<HostStatus>(NDO_DATA_CHECKTYPE,
        &HostStatus::check_type),
      KeySetter<HostStatus>(NDO_DATA_CURRENTCHECKATTEMPT,
        &HostStatus::current_check_attempt),
      KeySetter<HostStatus>(NDO_DATA_CURRENTNOTIFICATIONNUMBER,
        &HostStatus::current_notification_number),
      KeySetter<HostStatus>(NDO_DATA_CURRENTSTATE,
        &HostStatus::current_state),
      KeySetter<HostStatus>(NDO_DATA_EVENTHANDLER,
        &HostStatus::event_handler),
      KeySetter<HostStatus>(NDO_DATA_EVENTHANDLERENABLED,
        &HostStatus::event_handler_enabled),
      KeySetter<HostStatus>(NDO_DATA_EXECUTIONTIME,
        &HostStatus::execution_time),
      KeySetter<HostStatus>(NDO_DATA_FAILUREPREDICTIONENABLED,
        &HostStatus::failure_prediction_enabled),
      KeySetter<HostStatus>(NDO_DATA_FLAPDETECTIONENABLED,
        &HostStatus::flap_detection_enabled),
      KeySetter<HostStatus>(NDO_DATA_HASBEENCHECKED,
        &HostStatus::has_been_checked),
      KeySetter<HostStatus>(NDO_DATA_HOST,
        &HostStatus::host),
      KeySetter<HostStatus>(NDO_DATA_HOSTCHECKPERIOD,
        &HostStatus::check_period),
      KeySetter<HostStatus>(NDO_DATA_ISFLAPPING,
        &HostStatus::is_flapping),
      KeySetter<HostStatus>(NDO_DATA_LASTHARDSTATE,
        &HostStatus::last_hard_state),
      KeySetter<HostStatus>(NDO_DATA_LASTHARDSTATECHANGE,
        &HostStatus::last_hard_state_change),
      KeySetter<HostStatus>(NDO_DATA_LASTHOSTCHECK,
        &HostStatus::last_check),
      KeySetter<HostStatus>(NDO_DATA_LASTHOSTNOTIFICATION,
        &HostStatus::last_notification),
      KeySetter<HostStatus>(NDO_DATA_LASTSTATECHANGE,
        &HostStatus::last_state_change),
      KeySetter<HostStatus>(NDO_DATA_LASTTIMEDOWN,
        &HostStatus::last_time_down),
      KeySetter<HostStatus>(NDO_DATA_LASTTIMEUNREACHABLE,
        &HostStatus::last_time_unreachable),
      KeySetter<HostStatus>(NDO_DATA_LATENCY,
        &HostStatus::latency),
      KeySetter<HostStatus>(NDO_DATA_LONGOUTPUT,
        &HostStatus::long_output),
      KeySetter<HostStatus>(NDO_DATA_MAXCHECKATTEMPTS,
        &HostStatus::max_check_attempts),
      KeySetter<HostStatus>(NDO_DATA_MODIFIEDHOSTATTRIBUTES,
        &HostStatus::modified_attributes),
      KeySetter<HostStatus>(NDO_DATA_NEXTHOSTCHECK,
        &HostStatus::next_check),
      KeySetter<HostStatus>(NDO_DATA_NEXTHOSTNOTIFICATION,
        &HostStatus::next_notification),
      KeySetter<HostStatus>(NDO_DATA_NOMORENOTIFICATIONS,
        &HostStatus::no_more_notifications),
      KeySetter<HostStatus>(NDO_DATA_NORMALCHECKINTERVAL,
        &HostStatus::check_interval),
      KeySetter<HostStatus>(NDO_DATA_NOTIFICATIONSENABLED,
        &HostStatus::notifications_enabled),
      KeySetter<HostStatus>(NDO_DATA_OBSESSOVERHOST,
        &HostStatus::obsess_over),
      KeySetter<HostStatus>(NDO_DATA_OUTPUT,
        &HostStatus::output),
      KeySetter<HostStatus>(NDO_DATA_PASSIVEHOSTCHECKSENABLED,
        &HostStatus::passive_checks_enabled),
      KeySetter<HostStatus>(NDO_DATA_PERCENTSTATECHANGE,
        &HostStatus::percent_state_change),
      KeySetter<HostStatus>(NDO_DATA_PERFDATA,
        &HostStatus::perf_data),
      KeySetter<HostStatus>(NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
        &HostStatus::problem_has_been_acknowledged),
      KeySetter<HostStatus>(NDO_DATA_PROCESSPERFORMANCEDATA,
        &HostStatus::process_performance_data),
      KeySetter<HostStatus>(NDO_DATA_RETRYCHECKINTERVAL,
        &HostStatus::retry_interval),
      KeySetter<HostStatus>(NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
        &HostStatus::scheduled_downtime_depth),
      KeySetter<HostStatus>(NDO_DATA_SHOULDBESCHEDULED,
        &HostStatus::should_be_scheduled),
      KeySetter<HostStatus>(NDO_DATA_STATETYPE,
        &HostStatus::state_type),
      KeySetter<HostStatus>()
    };

  HandleObject<HostStatus>(this->instance_, keys_setters, *this);
  return ;
}

/**
 *  Immediately after connection, handle the first data transmitted.
 */
void NetworkInput::HandleInitialization()
{
  char* key;
  char* tmp;
  const char* value;
  ProgramStatus* conn_info;

  conn_info = new ProgramStatus;
  conn_info->is_running = true;
  conn_info->program_start = time(NULL);
  key = this->GetLine();
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
        {
	  this->instance_ = value;
          conn_info->instance = value;
        }
      /*
      else if (!strcmp(key, NDO_API_AGENT))
	conn_info->agent_name = value;
      else if (!strcmp(key, NDO_API_AGENTVERSION))
	conn_info->agent_version = value;
      else if (!strcmp(key, NDO_API_CONNECTION))
	conn_info->connect_source = value;
      else if (!strcmp(key, NDO_API_CONNECTTYPE))
	conn_info->connect_type = value;
      */
      key = this->GetLine();
    }
  conn_info->instance = this->instance_;
  conn_info->program_start = time(NULL);
  EventPublisher::GetInstance().Publish(conn_info);
  return ;
}

/**
 *  Handle a log event and publish it against the EventPublisher.
 */
void NetworkInput::HandleLog()
{
  int key;
  char* key_str;
  Log* log;
  const char* value_str;
  char* tmp;

  log = new Log;
  log->instance = this->instance_;
  key_str = this->GetLine();
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
      if (NDO_DATA_LOGENTRY == key)
	{
	  char* data;
	  char* lasts;
	  char* value_s;

	  value_s = const_cast<char*>(value_str);
	  data = strtok_r(value_s, " ", &lasts);
	  log->c_time = strtol(data, NULL, 0);
	  data = strtok_r(NULL, ":", &lasts);
	  if (!strcmp(data, "SERVICE ALERT"))
	    {
	      log->msg_type = 0;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->service = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->type = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->retry = strtol(data, NULL, 0);
	      data = strtok_r(NULL, ";", &lasts);
	      log->output = data;
	    }
	  else if (!strcmp(data, "HOST ALERT"))
	    {
	      log->msg_type = 1;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->type = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->retry = strtol(data, NULL, 0);
	      data = strtok_r(NULL, ";", &lasts);
	      log->output = data;
	    }
	  else if (!strcmp(data, "SERVICE NOTIFICATION"))
	    {
	      log->msg_type = 2;
	      data = strtok_r(NULL, ";", &lasts);
	      log->notification_contact = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->service = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->notification_cmd = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->output = data;
	    }
	  else if (!strcmp(data, "HOST NOTIFICATION"))
	    {
	      log->msg_type = 3;
	      data = strtok_r(NULL, ";", &lasts);
	      log->notification_contact = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->notification_cmd = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->output = data;
	    }
	  else if (!strcmp(data, "CURRENT SERVICE STATE"))
	    {
	      log->msg_type = 6;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->service = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->type = data;
	    }
	  else if (!strcmp(data, "CURRENT HOST STATE"))
	    {
	      log->msg_type = 7;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->type = data;
	    }
	  else if (!strcmp(data, "INITIAL HOST STATE"))
	    {
	      log->msg_type = 9;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->type = data;
	    }
	  else if (!strcmp(data, "INITIAL SERVICE STATE"))
	    {
	      log->msg_type = 8;
	      data = strtok_r(NULL, ";", &lasts);
	      log->host = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->service = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->status = data;
	      data = strtok_r(NULL, ";", &lasts);
	      log->type = data;
	    }
	  else if (!strcmp(data, "EXTERNAL COMMAND"))
	    {
	      data = strtok_r(NULL, ";", &lasts);
	      if (!strcmp("ACKNOWLEDGE_SVC_PROBLEM", data))
		{
		  log->msg_type = 10;
		  data = strtok_r(NULL, ";", &lasts);
		  log->host = data;
		  data = strtok_r(NULL, ";", &lasts);
		  log->service = data;
		  data = strtok_r(NULL, ";", &lasts);
		  log->notification_contact = data;
		  data = strtok_r(NULL, ";", &lasts);
		  log->output = data;
		}
	      else if (!strcmp("ACKNOWLEDGE_HOST_PROBLEM", data))
		{
		  log->msg_type = 11;
		  data = strtok_r(NULL, ";", &lasts);
		  log->host = data;
		  data = strtok_r(NULL, ";", &lasts);
		  log->notification_contact = data;
		  data = strtok_r(NULL, ";", &lasts);
		  log->output = data;
		}
	      else
		; // XXX : problem
	    }
	  else if (!strcmp(data, "Warning"))
	    {
	      log->msg_type = 4;
	      data = strtok_r(NULL, "", &lasts);
	      log->output = data;
	    }
	  else
	    {
	      // XXX
	      log->msg_type = 5;
	      log->output = data;
	    }
	}
      key_str = this->GetLine();
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
  EventPublisher::GetInstance().Publish(log);
  return ;
}

/**
 *  Handle a program status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleProgramStatus()
{
  static const KeySetter<ProgramStatus> keys_setters[] =
    {
      KeySetter<ProgramStatus>(NDO_DATA_ACTIVEHOSTCHECKSENABLED,
        &ProgramStatus::active_host_checks_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_ACTIVESERVICECHECKSENABLED,
        &ProgramStatus::active_service_checks_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_DAEMONMODE,
        &ProgramStatus::daemon_mode),
      KeySetter<ProgramStatus>(NDO_DATA_EVENTHANDLERENABLED,
        &ProgramStatus::event_handler_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_FAILUREPREDICTIONENABLED,
        &ProgramStatus::failure_prediction_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_FLAPDETECTIONENABLED,
        &ProgramStatus::flap_detection_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_GLOBALHOSTEVENTHANDLER,
        &ProgramStatus::global_host_event_handler),
      KeySetter<ProgramStatus>(NDO_DATA_GLOBALSERVICEEVENTHANDLER,
        &ProgramStatus::global_service_event_handler),
      KeySetter<ProgramStatus>(NDO_DATA_LASTCOMMANDCHECK,
        &ProgramStatus::last_command_check),
      KeySetter<ProgramStatus>(NDO_DATA_LASTLOGROTATION,
        &ProgramStatus::last_log_rotation),
      KeySetter<ProgramStatus>(NDO_DATA_MODIFIEDHOSTATTRIBUTES,
        &ProgramStatus::modified_host_attributes),
      KeySetter<ProgramStatus>(NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
        &ProgramStatus::modified_service_attributes),
      KeySetter<ProgramStatus>(NDO_DATA_NOTIFICATIONSENABLED,
        &ProgramStatus::notifications_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_OBSESSOVERHOST,
        &ProgramStatus::obsess_over_hosts),
      KeySetter<ProgramStatus>(NDO_DATA_OBSESSOVERSERVICE,
        &ProgramStatus::obsess_over_services),
      KeySetter<ProgramStatus>(NDO_DATA_PASSIVEHOSTCHECKSENABLED,
        &ProgramStatus::passive_host_checks_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_PASSIVESERVICECHECKSENABLED,
        &ProgramStatus::passive_service_checks_enabled),
      KeySetter<ProgramStatus>(NDO_DATA_PROCESSPERFORMANCEDATA,
        &ProgramStatus::process_performance_data),
      KeySetter<ProgramStatus>(NDO_DATA_PROCESSID,
        &ProgramStatus::pid),
      KeySetter<ProgramStatus>(NDO_DATA_PROGRAMSTARTTIME,
        &ProgramStatus::program_start),
      KeySetter<ProgramStatus>()
    };

  HandleObject<ProgramStatus>(this->instance_, keys_setters, *this);
  return ;
}

/**
 *  Handle a service definition event and publish it against the
 *  EventPublisher.
 */
void NetworkInput::HandleService()
{
  static const KeySetter<Service> keys_setters[] =
    {
      KeySetter<Service>(NDO_DATA_ACTIONURL,
        &Service::action_url),
      KeySetter<Service>(NDO_DATA_ACTIVESERVICECHECKSENABLED,
        &Service::active_checks_enabled),
      KeySetter<Service>(NDO_DATA_CUSTOMVARIABLE,
        &CustomVarToService),
      KeySetter<Service>(NDO_DATA_DISPLAYNAME,
        &Service::display_name),
      KeySetter<Service>(NDO_DATA_FIRSTNOTIFICATIONDELAY,
        &Service::first_notification_delay),
      KeySetter<Service>(NDO_DATA_FLAPDETECTIONONCRITICAL,
        &Service::flap_detection_on_critical),
      KeySetter<Service>(NDO_DATA_FLAPDETECTIONONOK,
        &Service::flap_detection_on_ok),
      KeySetter<Service>(NDO_DATA_FLAPDETECTIONONUNKNOWN,
        &Service::flap_detection_on_unknown),
      KeySetter<Service>(NDO_DATA_FLAPDETECTIONONWARNING,
        &Service::flap_detection_on_warning),
      KeySetter<Service>(NDO_DATA_HIGHSERVICEFLAPTHRESHOLD,
        &Service::high_flap_threshold),
      KeySetter<Service>(NDO_DATA_HOSTNAME,
        &Service::host),
      KeySetter<Service>(NDO_DATA_ICONIMAGE,
        &Service::icon_image),
      KeySetter<Service>(NDO_DATA_ICONIMAGEALT,
        &Service::icon_image_alt),
      KeySetter<Service>(NDO_DATA_LOWSERVICEFLAPTHRESHOLD,
        &Service::low_flap_threshold),
      KeySetter<Service>(NDO_DATA_MAXSERVICECHECKATTEMPTS,
        &Service::max_check_attempts),
      KeySetter<Service>(NDO_DATA_NOTES,
        &Service::notes),
      KeySetter<Service>(NDO_DATA_NOTESURL,
        &Service::notes_url),
      KeySetter<Service>(NDO_DATA_NOTIFYSERVICECRITICAL,
        &Service::notified_on_critical),
      KeySetter<Service>(NDO_DATA_NOTIFYSERVICEDOWNTIME,
        &Service::notify_on_downtime),
      KeySetter<Service>(NDO_DATA_NOTIFYSERVICEFLAPPING,
        &Service::notify_on_flapping),
      KeySetter<Service>(NDO_DATA_NOTIFYSERVICERECOVERY,
        &Service::notify_on_recovery),
      KeySetter<Service>(NDO_DATA_NOTIFYSERVICEUNKNOWN,
        &Service::notified_on_unknown),
      KeySetter<Service>(NDO_DATA_NOTIFYSERVICEWARNING,
        &Service::notified_on_warning),
      KeySetter<Service>(NDO_DATA_OBSESSOVERSERVICE,
        &Service::obsess_over),
      KeySetter<Service>(NDO_DATA_PASSIVESERVICECHECKSENABLED,
        &Service::passive_checks_enabled),
      KeySetter<Service>(NDO_DATA_PROCESSSERVICEPERFORMANCEDATA,
        &Service::process_performance_data),
      KeySetter<Service>(NDO_DATA_RETAINSERVICENONSTATUSINFORMATION,
        &Service::retain_nonstatus_information),
      KeySetter<Service>(NDO_DATA_RETAINSERVICESTATUSINFORMATION,
        &Service::retain_status_information),
      KeySetter<Service>(NDO_DATA_SERVICECHECKCOMMAND,
        &Service::check_command),
      KeySetter<Service>(NDO_DATA_SERVICECHECKINTERVAL,
        &Service::check_interval),
      KeySetter<Service>(NDO_DATA_SERVICECHECKPERIOD,
        &Service::check_period),
      KeySetter<Service>(NDO_DATA_SERVICEDESCRIPTION,
        &Service::service),
      KeySetter<Service>(NDO_DATA_SERVICEEVENTHANDLER,
        &Service::event_handler),
      KeySetter<Service>(NDO_DATA_SERVICEEVENTHANDLERENABLED,
        &Service::event_handler_enabled),
      KeySetter<Service>(NDO_DATA_SERVICEFAILUREPREDICTIONENABLED,
        &Service::failure_prediction_enabled),
      KeySetter<Service>(NDO_DATA_SERVICEFAILUREPREDICTIONOPTIONS,
        &Service::failure_prediction_options),
      KeySetter<Service>(NDO_DATA_SERVICEFLAPDETECTIONENABLED,
        &Service::flap_detection_enabled),
      KeySetter<Service>(NDO_DATA_SERVICEFRESHNESSCHECKSENABLED,
        &Service::check_freshness),
      KeySetter<Service>(NDO_DATA_SERVICEFRESHNESSTHRESHOLD,
        &Service::freshness_threshold),
      KeySetter<Service>(NDO_DATA_SERVICEISVOLATILE,
        &Service::is_volatile),
      KeySetter<Service>(NDO_DATA_SERVICENOTIFICATIONINTERVAL,
        &Service::notification_interval),
      KeySetter<Service>(NDO_DATA_SERVICENOTIFICATIONPERIOD,
        &Service::notification_period),
      KeySetter<Service>(NDO_DATA_SERVICENOTIFICATIONSENABLED,
        &Service::notifications_enabled),
      KeySetter<Service>(NDO_DATA_SERVICERETRYINTERVAL,
        &Service::retry_interval),
      KeySetter<Service>(NDO_DATA_STALKSERVICEONCRITICAL,
        &Service::stalk_on_critical),
      KeySetter<Service>(NDO_DATA_STALKSERVICEONOK,
        &Service::stalk_on_ok),
      KeySetter<Service>(NDO_DATA_STALKSERVICEONUNKNOWN,
        &Service::stalk_on_unknown),
      KeySetter<Service>(NDO_DATA_STALKSERVICEONWARNING,
        &Service::stalk_on_warning),
      KeySetter<Service>()
    };

  HandleObject<Service>(this->instance_, keys_setters, *this);
  return ;
}

/**
 *  Handle a service status event and publish it against the EventPublisher.
 */
void NetworkInput::HandleServiceStatus()
{
  static const KeySetter<ServiceStatus> keys_setters[] =
    {
      KeySetter<ServiceStatus>(NDO_DATA_ACKNOWLEDGEMENTTYPE,
        &ServiceStatus::acknowledgement_type),
      KeySetter<ServiceStatus>(NDO_DATA_ACTIVESERVICECHECKSENABLED,
        &ServiceStatus::active_checks_enabled),
      KeySetter<ServiceStatus>(NDO_DATA_CHECKCOMMAND,
        &ServiceStatus::check_command),
      KeySetter<ServiceStatus>(NDO_DATA_CHECKTYPE,
        &ServiceStatus::check_type),
      KeySetter<ServiceStatus>(NDO_DATA_CURRENTCHECKATTEMPT,
        &ServiceStatus::current_check_attempt),
      KeySetter<ServiceStatus>(NDO_DATA_CURRENTNOTIFICATIONNUMBER,
        &ServiceStatus::current_notification_number),
      KeySetter<ServiceStatus>(NDO_DATA_CURRENTSTATE,
        &ServiceStatus::current_state),
      KeySetter<ServiceStatus>(NDO_DATA_EVENTHANDLER,
        &ServiceStatus::event_handler),
      KeySetter<ServiceStatus>(NDO_DATA_EVENTHANDLERENABLED,
        &ServiceStatus::event_handler_enabled),
      KeySetter<ServiceStatus>(NDO_DATA_EXECUTIONTIME,
        &ServiceStatus::execution_time),
      KeySetter<ServiceStatus>(NDO_DATA_FAILUREPREDICTIONENABLED,
        &ServiceStatus::failure_prediction_enabled),
      KeySetter<ServiceStatus>(NDO_DATA_FLAPDETECTIONENABLED,
        &ServiceStatus::flap_detection_enabled),
      KeySetter<ServiceStatus>(NDO_DATA_HASBEENCHECKED,
        &ServiceStatus::has_been_checked),
      KeySetter<ServiceStatus>(NDO_DATA_HOST,
        &ServiceStatus::host),
      KeySetter<ServiceStatus>(NDO_DATA_ISFLAPPING,
        &ServiceStatus::is_flapping),
      KeySetter<ServiceStatus>(NDO_DATA_LASTSERVICECHECK,
        &ServiceStatus::last_check),
      KeySetter<ServiceStatus>(NDO_DATA_LASTHARDSTATE,
        &ServiceStatus::last_hard_state),
      KeySetter<ServiceStatus>(NDO_DATA_LASTHARDSTATECHANGE,
        &ServiceStatus::last_hard_state_change),
      KeySetter<ServiceStatus>(NDO_DATA_LASTSERVICENOTIFICATION,
        &ServiceStatus::last_notification),
      KeySetter<ServiceStatus>(NDO_DATA_LASTSTATECHANGE,
        &ServiceStatus::last_state_change),
      KeySetter<ServiceStatus>(NDO_DATA_LASTTIMECRITICAL,
        &ServiceStatus::last_time_critical),
      KeySetter<ServiceStatus>(NDO_DATA_LASTTIMEOK,
        &ServiceStatus::last_time_ok),
      KeySetter<ServiceStatus>(NDO_DATA_LASTTIMEUNKNOWN,
        &ServiceStatus::last_time_unknown),
      KeySetter<ServiceStatus>(NDO_DATA_LASTTIMEWARNING,
        &ServiceStatus::last_time_warning),
      KeySetter<ServiceStatus>(NDO_DATA_LATENCY,
        &ServiceStatus::latency),
      KeySetter<ServiceStatus>(NDO_DATA_LONGOUTPUT,
        &ServiceStatus::long_output),
      KeySetter<ServiceStatus>(NDO_DATA_MAXCHECKATTEMPTS,
        &ServiceStatus::max_check_attempts),
      KeySetter<ServiceStatus>(NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
        &ServiceStatus::modified_attributes),
      KeySetter<ServiceStatus>(NDO_DATA_NEXTSERVICECHECK,
        &ServiceStatus::next_check),
      KeySetter<ServiceStatus>(NDO_DATA_NEXTSERVICENOTIFICATION,
        &ServiceStatus::next_notification),
      KeySetter<ServiceStatus>(NDO_DATA_NOMORENOTIFICATIONS,
        &ServiceStatus::no_more_notifications),
      KeySetter<ServiceStatus>(NDO_DATA_NORMALCHECKINTERVAL,
        &ServiceStatus::check_interval),
      KeySetter<ServiceStatus>(NDO_DATA_OBSESSOVERSERVICE,
        &ServiceStatus::obsess_over),
      KeySetter<ServiceStatus>(NDO_DATA_OUTPUT,
        &ServiceStatus::output),
      KeySetter<ServiceStatus>(NDO_DATA_PASSIVESERVICECHECKSENABLED,
        &ServiceStatus::passive_checks_enabled),
      KeySetter<ServiceStatus>(NDO_DATA_PERCENTSTATECHANGE,
        &ServiceStatus::percent_state_change),
      KeySetter<ServiceStatus>(NDO_DATA_PERFDATA,
        &ServiceStatus::perf_data),
      KeySetter<ServiceStatus>(NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
        &ServiceStatus::problem_has_been_acknowledged),
      KeySetter<ServiceStatus>(NDO_DATA_PROCESSPERFORMANCEDATA,
        &ServiceStatus::process_performance_data),
      KeySetter<ServiceStatus>(NDO_DATA_RETRYCHECKINTERVAL,
        &ServiceStatus::retry_interval),
      KeySetter<ServiceStatus>(NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
        &ServiceStatus::scheduled_downtime_depth),
      KeySetter<ServiceStatus>(NDO_DATA_SERVICE,
        &ServiceStatus::service),
      KeySetter<ServiceStatus>(NDO_DATA_SERVICECHECKPERIOD,
        &ServiceStatus::check_period),
      KeySetter<ServiceStatus>(NDO_DATA_SHOULDBESCHEDULED,
        &ServiceStatus::should_be_scheduled),
      KeySetter<ServiceStatus>(NDO_DATA_STATETYPE,
        &ServiceStatus::state_type),
      KeySetter<ServiceStatus>()
    };

  HandleObject<ServiceStatus>(this->instance_, keys_setters, *this);
  return ;
}

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

  boost::unique_lock<boost::mutex> lock(this->threadm_);

  if (this->thread_)
    {
      this->socket_->Close();
      this->thread_->join();
      delete (this->thread_);
      delete (this->socket_);
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
    void (NetworkInput::* handler)();
  } handlers[] =
      {
	{ NDO_API_ACKNOWLEDGEMENTDATA, &NetworkInput::HandleAcknowledgement },
	{ NDO_API_COMMENTDATA, &NetworkInput::HandleComment },
	{ NDO_API_DOWNTIMEDATA, &NetworkInput::HandleDowntime },
	{ NDO_API_HOSTDEFINITION, &NetworkInput::HandleHost },
	{ NDO_API_HOSTGROUPDEFINITION, &NetworkInput::HandleHostGroup },
	{ NDO_API_HOSTSTATUSDATA, &NetworkInput::HandleHostStatus },
	{ NDO_API_LOGDATA, &NetworkInput::HandleLog },
	{ NDO_API_PROGRAMSTATUSDATA, &NetworkInput::HandleProgramStatus },
	{ NDO_API_SERVICEDEFINITION, &NetworkInput::HandleService },
	{ NDO_API_SERVICESTATUSDATA, &NetworkInput::HandleServiceStatus },
	{ 0, NULL }
      };

  try
    {
      this->HandleInitialization();
      buffer = this->GetLine();
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
		    (this->*(handlers[i].handler))();
		    break ;
		  }
	    }
	  buffer = this->GetLine();
	}
    }
  catch (std::exception& e)
    {
      logging.LogError("Exception occured while processing network input");
      logging.LogError(e.what());
    }
  catch (...)
    {
      logging.LogError(
        "Unknown exception occured while processing network input");
    }
  if (this->threadm_.try_lock())
    {
      this->socket_->Close();
      delete (this->socket_);
      this->socket_ = NULL;
      this->thread_->detach();
      delete (this->thread_);
      this->thread_ = NULL;
      this->threadm_.unlock();
      this->parent_->CleanupNetworkInput(this);
    }
  logging.LogInfo("Exiting input processing thread...");
  return ;
}

char* NetworkInput::GetLine()
{
  int old_length;

  this->length_ -= this->discard_;
  memmove(this->buffer_,
	  this->buffer_ + this->discard_,
	  this->length_ + 1);
  this->discard_ = 0;
  old_length = 0;
  while (!strchr(this->buffer_ + old_length, '\n')
	 && this->length_ < sizeof(this->buffer_) - 1)
    {
      unsigned long bytes_read;

      old_length = this->length_;
      bytes_read = this->socket_->Receive(this->buffer_ + this->length_,
                     sizeof(this->buffer_) - this->length_ - 1);
      this->length_ += bytes_read;
      this->buffer_[this->length_] = '\0';
      // XXX : find a better way to correct this
      if (!bytes_read)
	throw (Exception(0, "Socket is closed"));
    }
  this->discard_ = strcspn(this->buffer_, "\n");
  this->buffer_[this->discard_++] = '\0';
  return (this->buffer_);
}
