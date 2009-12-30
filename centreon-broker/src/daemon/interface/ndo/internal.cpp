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

#include <sstream>
#include <stdlib.h>                 // for strtol
#include <string.h>                 // for strcspn, strdup, strncmp
#include "events/events.h"
#include "exception.h"
#include "interface/ndo/internal.h"
#include "nagios/protoapi.h"

using namespace Events;
using namespace Interface::NDO;

/**************************************
*                                     *
*          Static Functions           *
*                                     *
**************************************/

/**
 *  Get all members of the host group.
 */
std::string GetHostGroupMembers(const HostGroup& host_group)
{
  std::string result;
  std::list<std::string>::const_iterator end;

  end = host_group.members.end();
  for (std::list<std::string>::const_iterator it = host_group.members.begin();
       it != end;
       ++it)
    {
      std::stringstream ss;

      ss << NDO_DATA_HOSTGROUPMEMBER << '=' << (*it) << '\n';
      result.append(ss.str());
    }
  return (result);
}

/**
 *  Append a member to the host group.
 */
static void SetHostGroupMember(HostGroup& host_group, const char* member)
{
  host_group.members.push_back(member);
  return ;
}

/**
 *  Extract NDO-formated log data to the C++ object.
 */
#define LOGDATA_EXTRACT data = strtok_r(NULL, ";", &lasts);                   \
                        if (!data)                                            \
                          throw (Exception(0, "Log data extraction failed."));
static void SetLogData(Log& log, const char* data)
{
  char* datadup;

  datadup = strdup(data);
  if (!datadup)
    throw (Exception(0, "Log data extraction failed."));
  try
    {
      char* data;
      char* lasts;

      data = strtok_r(datadup, " ", &lasts);
      if (!data)
        return ;
      log.c_time = strtol(data, NULL, 0);
      data = strtok_r(NULL, ":", &lasts);
      if (!data)
	throw (Exception(0, "Log data extraction failed."));
      if (!strcmp(data, "SERVICE ALERT"))
	{
	  log.msg_type = 0;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.service = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.type = data;
	  LOGDATA_EXTRACT;
	  log.retry = strtol(data, NULL, 0);
	  LOGDATA_EXTRACT;
	  log.output = data;
	}
      else if (!strcmp(data, "HOST ALERT"))
	{
	  log.msg_type = 1;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.type = data;
	  LOGDATA_EXTRACT;
	  log.retry = strtol(data, NULL, 0);
	  LOGDATA_EXTRACT;
	  log.output = data;
	}
      else if (!strcmp(data, "SERVICE NOTIFICATION"))
	{
	  log.msg_type = 2;
	  LOGDATA_EXTRACT;
	  log.notification_contact = data;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.service = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.notification_cmd = data;
	  LOGDATA_EXTRACT;
	  log.output = data;
	}
      else if (!strcmp(data, "HOST NOTIFICATION"))
	{
	  log.msg_type = 3;
	  LOGDATA_EXTRACT;
	  log.notification_contact = data;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.notification_cmd = data;
	  LOGDATA_EXTRACT;
	  log.output = data;
	}
      else if (!strcmp(data, "CURRENT SERVICE STATE"))
	{
	  log.msg_type = 6;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.service = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.type = data;
	}
      else if (!strcmp(data, "CURRENT HOST STATE"))
	{
	  log.msg_type = 7;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.type = data;
	}
      else if (!strcmp(data, "INITIAL HOST STATE"))
	{
	  log.msg_type = 9;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.type = data;
	}
      else if (!strcmp(data, "INITIAL SERVICE STATE"))
	{
	  log.msg_type = 8;
	  LOGDATA_EXTRACT;
	  log.host = data;
	  LOGDATA_EXTRACT;
	  log.service = data;
	  LOGDATA_EXTRACT;
	  log.status = data;
	  LOGDATA_EXTRACT;
	  log.type = data;
	}
      else if (!strcmp(data, "EXTERNAL_COMMAND"))
	{
	  LOGDATA_EXTRACT;
	  if (!strcmp(data, "ACKNOWLEDGE_SVC_PROBLEM"))
	    {
	      log.msg_type = 10;
	      LOGDATA_EXTRACT;
	      log.host = data;
	      LOGDATA_EXTRACT;
	      log.service = data;
	      LOGDATA_EXTRACT;
	      log.notification_contact = data;
	      LOGDATA_EXTRACT;
	      log.output = data;
	    }
	  else if (!strcmp(data, "ACKNOWLEDGE_HOST_PROBLEM"))
	    {
	      log.msg_type = 11;
	      LOGDATA_EXTRACT;
	      log.host = data;
	      LOGDATA_EXTRACT;
	      log.notification_contact = data;
	      LOGDATA_EXTRACT;
	      log.output = data;
	    }
	  else
	    throw (Exception(0, "Log data extraction failed."));
	}
      else if (!strcmp(data, "Warning"))
	{
	  log.msg_type = 4;
	  data = strtok_r(NULL, "", &lasts);
	  if (!data)
	    throw (Exception(0, "Log data extraction failed."));
	  log.output = data;
	}
      else
	{
	  log.msg_type = 5;
	  log.output = datadup;
	}
    }
  catch (...) {}
  free(datadup);
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
*            Global Arrays            *
*                                     *
**************************************/

// Acknowledgement fields.
const KeyField<Acknowledgement> Interface::NDO::acknowledgement_fields[] =
  {
    KeyField<Acknowledgement>(NDO_DATA_ACKNOWLEDGEMENTTYPE,
      &Acknowledgement::acknowledgement_type),
    KeyField<Acknowledgement>(NDO_DATA_AUTHORNAME,
      &Acknowledgement::author),
    KeyField<Acknowledgement>(NDO_DATA_COMMENT,
      &Acknowledgement::comment),
    KeyField<Acknowledgement>(NDO_DATA_HOST,
      &Acknowledgement::host),
    KeyField<Acknowledgement>(NDO_DATA_INSTANCE,
      &Acknowledgement::instance),
    KeyField<Acknowledgement>(NDO_DATA_NOTIFYCONTACTS,
      &Acknowledgement::notify_contacts),
    KeyField<Acknowledgement>(NDO_DATA_PERSISTENT,
      &Acknowledgement::persistent_comment),
    KeyField<Acknowledgement>(NDO_DATA_SERVICE,
      &Acknowledgement::service),
    KeyField<Acknowledgement>(NDO_DATA_STATE,
      &Acknowledgement::state),
    KeyField<Acknowledgement>(NDO_DATA_STICKY,
      &Acknowledgement::is_sticky),
    KeyField<Acknowledgement>(NDO_DATA_TIMESTAMP,
      &Acknowledgement::entry_time),
    KeyField<Acknowledgement>(NDO_DATA_TYPE,
      &Acknowledgement::type),
    KeyField<Acknowledgement>()
  };

// Comment fields.
const KeyField<Comment> Interface::NDO::comment_fields[] =
  {
    KeyField<Comment>(NDO_DATA_AUTHORNAME,
      &Comment::author),
    KeyField<Comment>(NDO_DATA_COMMENT,
      &Comment::comment),
    KeyField<Comment>(NDO_DATA_COMMENTID,
      &Comment::internal_id),
    KeyField<Comment>(NDO_DATA_COMMENTTYPE,
      &Comment::comment_type),
    KeyField<Comment>(NDO_DATA_ENTRYTIME,
      &Comment::entry_time),
    KeyField<Comment>(NDO_DATA_ENTRYTYPE,
      &Comment::entry_type),
    KeyField<Comment>(NDO_DATA_EXPIRATIONTIME,
      &Comment::expire_time),
    KeyField<Comment>(NDO_DATA_EXPIRES,
      &Comment::expires),
    KeyField<Comment>(NDO_DATA_HOST,
      &Comment::host),
    KeyField<Comment>(NDO_DATA_INSTANCE,
      &Comment::instance),
    KeyField<Comment>(NDO_DATA_PERSISTENT,
      &Comment::persistent),
    KeyField<Comment>(NDO_DATA_SERVICE,
       &Comment::service),
    KeyField<Comment>(NDO_DATA_SOURCE,
      &Comment::source),
    KeyField<Comment>(NDO_DATA_TYPE,
      &Comment::type),
    KeyField<Comment>()
  };

// Downtime fields.
const KeyField<Downtime> Interface::NDO::downtime_fields[] =
  {
    KeyField<Downtime>(NDO_DATA_AUTHORNAME,
      &Downtime::author),
    KeyField<Downtime>(NDO_DATA_COMMENT,
      &Downtime::comment),
    KeyField<Downtime>(NDO_DATA_DOWNTIMEID,
      &Downtime::id),
    KeyField<Downtime>(NDO_DATA_DOWNTIMETYPE,
      &Downtime::downtime_type),
    KeyField<Downtime>(NDO_DATA_DURATION,
      &Downtime::duration),
    KeyField<Downtime>(NDO_DATA_ENDTIME,
      &Downtime::end_time),
    KeyField<Downtime>(NDO_DATA_ENTRYTIME,
      &Downtime::entry_time),
    KeyField<Downtime>(NDO_DATA_FIXED,
      &Downtime::fixed),
    KeyField<Downtime>(NDO_DATA_HOST,
      &Downtime::host),
    KeyField<Downtime>(NDO_DATA_INSTANCE,
      &Downtime::instance),
    KeyField<Downtime>(NDO_DATA_SERVICE,
      &Downtime::service),
    KeyField<Downtime>(NDO_DATA_STARTTIME,
      &Downtime::start_time),
    KeyField<Downtime>(NDO_DATA_TRIGGEREDBY,
      &Downtime::triggered_by),
    KeyField<Downtime>(NDO_DATA_TYPE,
      &Downtime::type),
    KeyField<Downtime>()
  };

// Host fields.
const KeyField<Host> Interface::NDO::host_fields[] =
  {
    KeyField<Host>(NDO_DATA_ACTIONURL,
      &Host::action_url),
    KeyField<Host>(NDO_DATA_ACTIVEHOSTCHECKSENABLED,
      &Host::active_checks_enabled),
    KeyField<Host>(NDO_DATA_CUSTOMVARIABLE,
      NULL,
      &CustomVarToHost),
    KeyField<Host>(NDO_DATA_DISPLAYNAME,
      &Host::display_name),
    KeyField<Host>(NDO_DATA_FIRSTNOTIFICATIONDELAY,
      &Host::first_notification_delay),
    KeyField<Host>(NDO_DATA_FLAPDETECTIONONDOWN,
      &Host::flap_detection_on_down),
    KeyField<Host>(NDO_DATA_FLAPDETECTIONONUNREACHABLE,
      &Host::flap_detection_on_unreachable),
    KeyField<Host>(NDO_DATA_FLAPDETECTIONONUP,
      &Host::flap_detection_on_up),
    KeyField<Host>(NDO_DATA_HAVE2DCOORDS,
      &Host::have_2d_coords),
    //{ NDO_DATA_HAVE3DCOORDS }
    KeyField<Host>(NDO_DATA_HIGHHOSTFLAPTHRESHOLD,
      &Host::high_flap_threshold),
    KeyField<Host>(NDO_DATA_HOSTADDRESS,
      &Host::address),
    KeyField<Host>(NDO_DATA_HOSTALIAS,
      &Host::alias),
    KeyField<Host>(NDO_DATA_HOSTCHECKCOMMAND,
      &Host::check_command),
    KeyField<Host>(NDO_DATA_HOSTCHECKINTERVAL,
      &Host::check_interval),
    KeyField<Host>(NDO_DATA_HOSTCHECKPERIOD,
      &Host::check_period),
    KeyField<Host>(NDO_DATA_HOSTEVENTHANDLER,
      &Host::event_handler),
    KeyField<Host>(NDO_DATA_HOSTEVENTHANDLERENABLED,
      &Host::event_handler_enabled),
    KeyField<Host>(NDO_DATA_HOSTFAILUREPREDICTIONENABLED,
      &Host::failure_prediction_enabled),
    //{ NDO_DATA_HOSTFAILUREPREDICTIONOPTIONS },
    KeyField<Host>(NDO_DATA_HOSTFLAPDETECTIONENABLED,
      &Host::flap_detection_enabled),
    KeyField<Host>(NDO_DATA_HOSTFRESHNESSCHECKSENABLED,
      &Host::check_freshness),
    KeyField<Host>(NDO_DATA_HOSTFRESHNESSTHRESHOLD,
      &Host::freshness_threshold),
    KeyField<Host>(NDO_DATA_HOSTMAXCHECKATTEMPTS,
      &Host::max_check_attempts),
    KeyField<Host>(NDO_DATA_HOSTNAME,
      &Host::host),
    KeyField<Host>(NDO_DATA_HOSTNOTIFICATIONINTERVAL,
      &Host::notification_interval),
    KeyField<Host>(NDO_DATA_HOSTNOTIFICATIONPERIOD,
      &Host::notification_period),
    KeyField<Host>(NDO_DATA_HOSTNOTIFICATIONSENABLED,
      &Host::notifications_enabled),
    KeyField<Host>(NDO_DATA_HOSTRETRYINTERVAL,
      &Host::retry_interval),
    KeyField<Host>(NDO_DATA_ICONIMAGE,
      &Host::icon_image),
    KeyField<Host>(NDO_DATA_ICONIMAGEALT,
      &Host::icon_image_alt),
    KeyField<Host>(NDO_DATA_INSTANCE,
      &Host::instance),
    KeyField<Host>(NDO_DATA_LOWHOSTFLAPTHRESHOLD,
      &Host::low_flap_threshold),
    KeyField<Host>(NDO_DATA_NOTES,
      &Host::notes),
    KeyField<Host>(NDO_DATA_NOTESURL,
      &Host::notes_url),
    KeyField<Host>(NDO_DATA_NOTIFYHOSTDOWN,
      &Host::notify_on_down),
    KeyField<Host>(NDO_DATA_NOTIFYHOSTDOWNTIME,
      &Host::notify_on_downtime),
    KeyField<Host>(NDO_DATA_NOTIFYHOSTFLAPPING,
      &Host::notify_on_flapping),
    KeyField<Host>(NDO_DATA_NOTIFYHOSTRECOVERY,
      &Host::notify_on_recovery),
    KeyField<Host>(NDO_DATA_NOTIFYHOSTUNREACHABLE,
      &Host::notify_on_unreachable),
    KeyField<Host>(NDO_DATA_OBSESSOVERHOST,
      &Host::obsess_over),
    KeyField<Host>(NDO_DATA_PASSIVEHOSTCHECKSENABLED,
      &Host::passive_checks_enabled),
    KeyField<Host>(NDO_DATA_PROCESSHOSTPERFORMANCEDATA,
      &Host::process_performance_data),
    KeyField<Host>(NDO_DATA_RETAINHOSTNONSTATUSINFORMATION,
      &Host::retain_nonstatus_information),
    KeyField<Host>(NDO_DATA_RETAINHOSTSTATUSINFORMATION,
      &Host::retain_status_information),
    KeyField<Host>(NDO_DATA_STALKHOSTONDOWN,
      &Host::stalk_on_down),
    KeyField<Host>(NDO_DATA_STALKHOSTONUNREACHABLE,
      &Host::stalk_on_unreachable),
    KeyField<Host>(NDO_DATA_STALKHOSTONUP,
      &Host::stalk_on_up),
    KeyField<Host>(NDO_DATA_STATUSMAPIMAGE,
      &Host::statusmap_image),
    KeyField<Host>(NDO_DATA_VRMLIMAGE,
      &Host::vrml_image),
    KeyField<Host>(NDO_DATA_X2D,
      &Host::x_2d),
    //{ NDO_DATA_X3D },
    KeyField<Host>(NDO_DATA_Y2D,
      &Host::y_2d),
    //{ NDO_DATA_Y3D },
    //{ NDO_DATA_Z3D },
    KeyField<Host>()
  };

// HostGroup fields.
const KeyField<HostGroup> Interface::NDO::host_group_fields[] =
  {
    KeyField<HostGroup>(NDO_DATA_HOSTGROUPALIAS,
      &HostGroup::alias),
    KeyField<HostGroup>(NDO_DATA_HOSTGROUPMEMBER,
      &GetHostGroupMembers,
      &SetHostGroupMember),
    KeyField<HostGroup>(NDO_DATA_HOSTGROUPNAME,
      &HostGroup::name),
    KeyField<HostGroup>(NDO_DATA_INSTANCE,
      &HostGroup::instance),
    KeyField<HostGroup>()
  };

// HostStatus fields.
const KeyField<HostStatus> Interface::NDO::host_status_fields[] =
  {
    KeyField<HostStatus>(NDO_DATA_ACKNOWLEDGEMENTTYPE,
      &HostStatus::acknowledgement_type),
    KeyField<HostStatus>(NDO_DATA_ACTIVEHOSTCHECKSENABLED,
      &HostStatus::active_checks_enabled),
    KeyField<HostStatus>(NDO_DATA_CHECKCOMMAND,
     &HostStatus::check_command),
    KeyField<HostStatus>(NDO_DATA_CHECKTYPE,
      &HostStatus::check_type),
    KeyField<HostStatus>(NDO_DATA_CURRENTCHECKATTEMPT,
      &HostStatus::current_check_attempt),
    KeyField<HostStatus>(NDO_DATA_CURRENTNOTIFICATIONNUMBER,
      &HostStatus::current_notification_number),
    KeyField<HostStatus>(NDO_DATA_CURRENTSTATE,
      &HostStatus::current_state),
    KeyField<HostStatus>(NDO_DATA_EVENTHANDLER,
      &HostStatus::event_handler),
    KeyField<HostStatus>(NDO_DATA_EVENTHANDLERENABLED,
      &HostStatus::event_handler_enabled),
    KeyField<HostStatus>(NDO_DATA_EXECUTIONTIME,
      &HostStatus::execution_time),
    KeyField<HostStatus>(NDO_DATA_FAILUREPREDICTIONENABLED,
      &HostStatus::failure_prediction_enabled),
    KeyField<HostStatus>(NDO_DATA_FLAPDETECTIONENABLED,
      &HostStatus::flap_detection_enabled),
    KeyField<HostStatus>(NDO_DATA_HASBEENCHECKED,
      &HostStatus::has_been_checked),
    KeyField<HostStatus>(NDO_DATA_HOST,
      &HostStatus::host),
    KeyField<HostStatus>(NDO_DATA_HOSTCHECKPERIOD,
      &HostStatus::check_period),
    KeyField<HostStatus>(NDO_DATA_INSTANCE,
      &HostStatus::instance),
    KeyField<HostStatus>(NDO_DATA_ISFLAPPING,
      &HostStatus::is_flapping),
    KeyField<HostStatus>(NDO_DATA_LASTHARDSTATE,
      &HostStatus::last_hard_state),
    KeyField<HostStatus>(NDO_DATA_LASTHARDSTATECHANGE,
      &HostStatus::last_hard_state_change),
    KeyField<HostStatus>(NDO_DATA_LASTHOSTCHECK,
      &HostStatus::last_check),
    KeyField<HostStatus>(NDO_DATA_LASTHOSTNOTIFICATION,
      &HostStatus::last_notification),
    KeyField<HostStatus>(NDO_DATA_LASTSTATECHANGE,
      &HostStatus::last_state_change),
    KeyField<HostStatus>(NDO_DATA_LASTTIMEDOWN,
      &HostStatus::last_time_down),
    KeyField<HostStatus>(NDO_DATA_LASTTIMEUNREACHABLE,
      &HostStatus::last_time_unreachable),
    KeyField<HostStatus>(NDO_DATA_LATENCY,
      &HostStatus::latency),
    KeyField<HostStatus>(NDO_DATA_LONGOUTPUT,
      &HostStatus::long_output),
    KeyField<HostStatus>(NDO_DATA_MAXCHECKATTEMPTS,
      &HostStatus::max_check_attempts),
    KeyField<HostStatus>(NDO_DATA_MODIFIEDHOSTATTRIBUTES,
      &HostStatus::modified_attributes),
    KeyField<HostStatus>(NDO_DATA_NEXTHOSTCHECK,
      &HostStatus::next_check),
    KeyField<HostStatus>(NDO_DATA_NEXTHOSTNOTIFICATION,
      &HostStatus::next_notification),
    KeyField<HostStatus>(NDO_DATA_NOMORENOTIFICATIONS,
      &HostStatus::no_more_notifications),
    KeyField<HostStatus>(NDO_DATA_NORMALCHECKINTERVAL,
      &HostStatus::check_interval),
    KeyField<HostStatus>(NDO_DATA_NOTIFICATIONSENABLED,
      &HostStatus::notifications_enabled),
    KeyField<HostStatus>(NDO_DATA_OBSESSOVERHOST,
      &HostStatus::obsess_over),
    KeyField<HostStatus>(NDO_DATA_OUTPUT,
      &HostStatus::output),
    KeyField<HostStatus>(NDO_DATA_PASSIVEHOSTCHECKSENABLED,
      &HostStatus::passive_checks_enabled),
    KeyField<HostStatus>(NDO_DATA_PERCENTSTATECHANGE,
      &HostStatus::percent_state_change),
    KeyField<HostStatus>(NDO_DATA_PERFDATA,
      &HostStatus::perf_data),
    KeyField<HostStatus>(NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
      &HostStatus::problem_has_been_acknowledged),
    KeyField<HostStatus>(NDO_DATA_PROCESSPERFORMANCEDATA,
      &HostStatus::process_performance_data),
    KeyField<HostStatus>(NDO_DATA_RETRYCHECKINTERVAL,
      &HostStatus::retry_interval),
    KeyField<HostStatus>(NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
      &HostStatus::scheduled_downtime_depth),
    KeyField<HostStatus>(NDO_DATA_SHOULDBESCHEDULED,
      &HostStatus::should_be_scheduled),
    KeyField<HostStatus>(NDO_DATA_STATETYPE,
      &HostStatus::state_type),
    KeyField<HostStatus>()
  };

// Log fields.
const KeyField<Log> Interface::NDO::log_fields[] =
  {
    KeyField<Log>(NDO_DATA_INSTANCE,
      &Log::instance),
    KeyField<Log>(NDO_DATA_LOGENTRY,
                  NULL,
                  &SetLogData),
    KeyField<Log>()
  };

// ProgramStatus fields.
const KeyField<ProgramStatus> Interface::NDO::program_status_fields[] =
  {
    KeyField<ProgramStatus>(NDO_DATA_ACTIVEHOSTCHECKSENABLED,
      &ProgramStatus::active_host_checks_enabled),
    KeyField<ProgramStatus>(NDO_DATA_ACTIVESERVICECHECKSENABLED,
      &ProgramStatus::active_service_checks_enabled),
    KeyField<ProgramStatus>(NDO_DATA_DAEMONMODE,
      &ProgramStatus::daemon_mode),
    KeyField<ProgramStatus>(NDO_DATA_EVENTHANDLERENABLED,
      &ProgramStatus::event_handler_enabled),
    KeyField<ProgramStatus>(NDO_DATA_FAILUREPREDICTIONENABLED,
      &ProgramStatus::failure_prediction_enabled),
    KeyField<ProgramStatus>(NDO_DATA_FLAPDETECTIONENABLED,
      &ProgramStatus::flap_detection_enabled),
    KeyField<ProgramStatus>(NDO_DATA_GLOBALHOSTEVENTHANDLER,
      &ProgramStatus::global_host_event_handler),
    KeyField<ProgramStatus>(NDO_DATA_GLOBALSERVICEEVENTHANDLER,
      &ProgramStatus::global_service_event_handler),
    KeyField<ProgramStatus>(NDO_DATA_INSTANCE,
      &ProgramStatus::instance),
    KeyField<ProgramStatus>(NDO_DATA_LASTCOMMANDCHECK,
      &ProgramStatus::last_command_check),
    KeyField<ProgramStatus>(NDO_DATA_LASTLOGROTATION,
      &ProgramStatus::last_log_rotation),
    KeyField<ProgramStatus>(NDO_DATA_MODIFIEDHOSTATTRIBUTES,
      &ProgramStatus::modified_host_attributes),
    KeyField<ProgramStatus>(NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
      &ProgramStatus::modified_service_attributes),
    KeyField<ProgramStatus>(NDO_DATA_NOTIFICATIONSENABLED,
      &ProgramStatus::notifications_enabled),
    KeyField<ProgramStatus>(NDO_DATA_OBSESSOVERHOST,
      &ProgramStatus::obsess_over_hosts),
    KeyField<ProgramStatus>(NDO_DATA_OBSESSOVERSERVICE,
      &ProgramStatus::obsess_over_services),
    KeyField<ProgramStatus>(NDO_DATA_PASSIVEHOSTCHECKSENABLED,
      &ProgramStatus::passive_host_checks_enabled),
    KeyField<ProgramStatus>(NDO_DATA_PASSIVESERVICECHECKSENABLED,
      &ProgramStatus::passive_service_checks_enabled),
    KeyField<ProgramStatus>(NDO_DATA_PROCESSPERFORMANCEDATA,
      &ProgramStatus::process_performance_data),
    KeyField<ProgramStatus>(NDO_DATA_PROCESSID,
      &ProgramStatus::pid),
    KeyField<ProgramStatus>(NDO_DATA_PROGRAMSTARTTIME,
      &ProgramStatus::program_start),
    KeyField<ProgramStatus>()
  };

// Service fields.
const KeyField<Service> Interface::NDO::service_fields[] =
  {
    KeyField<Service>(NDO_DATA_ACTIONURL,
      &Service::action_url),
    KeyField<Service>(NDO_DATA_ACTIVESERVICECHECKSENABLED,
      &Service::active_checks_enabled),
    KeyField<Service>(NDO_DATA_CUSTOMVARIABLE,
      NULL,
      &CustomVarToService),
    KeyField<Service>(NDO_DATA_DISPLAYNAME,
      &Service::display_name),
    KeyField<Service>(NDO_DATA_FIRSTNOTIFICATIONDELAY,
      &Service::first_notification_delay),
    KeyField<Service>(NDO_DATA_FLAPDETECTIONONCRITICAL,
      &Service::flap_detection_on_critical),
    KeyField<Service>(NDO_DATA_FLAPDETECTIONONOK,
      &Service::flap_detection_on_ok),
    KeyField<Service>(NDO_DATA_FLAPDETECTIONONUNKNOWN,
      &Service::flap_detection_on_unknown),
    KeyField<Service>(NDO_DATA_FLAPDETECTIONONWARNING,
      &Service::flap_detection_on_warning),
    KeyField<Service>(NDO_DATA_HIGHSERVICEFLAPTHRESHOLD,
      &Service::high_flap_threshold),
    KeyField<Service>(NDO_DATA_HOSTNAME,
      &Service::host),
    KeyField<Service>(NDO_DATA_ICONIMAGE,
      &Service::icon_image),
    KeyField<Service>(NDO_DATA_ICONIMAGEALT,
      &Service::icon_image_alt),
    KeyField<Service>(NDO_DATA_INSTANCE,
      &Service::instance),
    KeyField<Service>(NDO_DATA_LOWSERVICEFLAPTHRESHOLD,
      &Service::low_flap_threshold),
    KeyField<Service>(NDO_DATA_MAXSERVICECHECKATTEMPTS,
      &Service::max_check_attempts),
    KeyField<Service>(NDO_DATA_NOTES,
      &Service::notes),
    KeyField<Service>(NDO_DATA_NOTESURL,
      &Service::notes_url),
    KeyField<Service>(NDO_DATA_NOTIFYSERVICECRITICAL,
      &Service::notified_on_critical),
    KeyField<Service>(NDO_DATA_NOTIFYSERVICEDOWNTIME,
      &Service::notify_on_downtime),
    KeyField<Service>(NDO_DATA_NOTIFYSERVICEFLAPPING,
      &Service::notify_on_flapping),
    KeyField<Service>(NDO_DATA_NOTIFYSERVICERECOVERY,
      &Service::notify_on_recovery),
    KeyField<Service>(NDO_DATA_NOTIFYSERVICEUNKNOWN,
      &Service::notified_on_unknown),
    KeyField<Service>(NDO_DATA_NOTIFYSERVICEWARNING,
      &Service::notified_on_warning),
    KeyField<Service>(NDO_DATA_OBSESSOVERSERVICE,
      &Service::obsess_over),
    KeyField<Service>(NDO_DATA_PASSIVESERVICECHECKSENABLED,
      &Service::passive_checks_enabled),
    KeyField<Service>(NDO_DATA_PROCESSSERVICEPERFORMANCEDATA,
      &Service::process_performance_data),
    KeyField<Service>(NDO_DATA_RETAINSERVICENONSTATUSINFORMATION,
      &Service::retain_nonstatus_information),
    KeyField<Service>(NDO_DATA_RETAINSERVICESTATUSINFORMATION,
      &Service::retain_status_information),
    KeyField<Service>(NDO_DATA_SERVICECHECKCOMMAND,
      &Service::check_command),
    KeyField<Service>(NDO_DATA_SERVICECHECKINTERVAL,
      &Service::check_interval),
    KeyField<Service>(NDO_DATA_SERVICECHECKPERIOD,
      &Service::check_period),
    KeyField<Service>(NDO_DATA_SERVICEDESCRIPTION,
      &Service::service),
    KeyField<Service>(NDO_DATA_SERVICEEVENTHANDLER,
      &Service::event_handler),
    KeyField<Service>(NDO_DATA_SERVICEEVENTHANDLERENABLED,
      &Service::event_handler_enabled),
    KeyField<Service>(NDO_DATA_SERVICEFAILUREPREDICTIONENABLED,
      &Service::failure_prediction_enabled),
    KeyField<Service>(NDO_DATA_SERVICEFAILUREPREDICTIONOPTIONS,
      &Service::failure_prediction_options),
    KeyField<Service>(NDO_DATA_SERVICEFLAPDETECTIONENABLED,
      &Service::flap_detection_enabled),
    KeyField<Service>(NDO_DATA_SERVICEFRESHNESSCHECKSENABLED,
      &Service::check_freshness),
    KeyField<Service>(NDO_DATA_SERVICEFRESHNESSTHRESHOLD,
      &Service::freshness_threshold),
    KeyField<Service>(NDO_DATA_SERVICEISVOLATILE,
      &Service::is_volatile),
    KeyField<Service>(NDO_DATA_SERVICENOTIFICATIONINTERVAL,
      &Service::notification_interval),
    KeyField<Service>(NDO_DATA_SERVICENOTIFICATIONPERIOD,
      &Service::notification_period),
    KeyField<Service>(NDO_DATA_SERVICENOTIFICATIONSENABLED,
      &Service::notifications_enabled),
    KeyField<Service>(NDO_DATA_SERVICERETRYINTERVAL,
      &Service::retry_interval),
    KeyField<Service>(NDO_DATA_STALKSERVICEONCRITICAL,
      &Service::stalk_on_critical),
    KeyField<Service>(NDO_DATA_STALKSERVICEONOK,
      &Service::stalk_on_ok),
    KeyField<Service>(NDO_DATA_STALKSERVICEONUNKNOWN,
      &Service::stalk_on_unknown),
    KeyField<Service>(NDO_DATA_STALKSERVICEONWARNING,
      &Service::stalk_on_warning),
    KeyField<Service>()
  };

// ServiceStatus fields.
const KeyField<ServiceStatus> Interface::NDO::service_status_fields[] =
  {
    KeyField<ServiceStatus>(NDO_DATA_ACKNOWLEDGEMENTTYPE,
      &ServiceStatus::acknowledgement_type),
    KeyField<ServiceStatus>(NDO_DATA_ACTIVESERVICECHECKSENABLED,
      &ServiceStatus::active_checks_enabled),
    KeyField<ServiceStatus>(NDO_DATA_CHECKCOMMAND,
      &ServiceStatus::check_command),
    KeyField<ServiceStatus>(NDO_DATA_CHECKTYPE,
      &ServiceStatus::check_type),
    KeyField<ServiceStatus>(NDO_DATA_CURRENTCHECKATTEMPT,
      &ServiceStatus::current_check_attempt),
    KeyField<ServiceStatus>(NDO_DATA_CURRENTNOTIFICATIONNUMBER,
      &ServiceStatus::current_notification_number),
    KeyField<ServiceStatus>(NDO_DATA_CURRENTSTATE,
      &ServiceStatus::current_state),
    KeyField<ServiceStatus>(NDO_DATA_EVENTHANDLER,
      &ServiceStatus::event_handler),
    KeyField<ServiceStatus>(NDO_DATA_EVENTHANDLERENABLED,
      &ServiceStatus::event_handler_enabled),
    KeyField<ServiceStatus>(NDO_DATA_EXECUTIONTIME,
      &ServiceStatus::execution_time),
    KeyField<ServiceStatus>(NDO_DATA_FAILUREPREDICTIONENABLED,
      &ServiceStatus::failure_prediction_enabled),
    KeyField<ServiceStatus>(NDO_DATA_FLAPDETECTIONENABLED,
      &ServiceStatus::flap_detection_enabled),
    KeyField<ServiceStatus>(NDO_DATA_HASBEENCHECKED,
      &ServiceStatus::has_been_checked),
    KeyField<ServiceStatus>(NDO_DATA_HOST,
      &ServiceStatus::host),
    KeyField<ServiceStatus>(NDO_DATA_INSTANCE,
      &ServiceStatus::instance),
    KeyField<ServiceStatus>(NDO_DATA_ISFLAPPING,
      &ServiceStatus::is_flapping),
    KeyField<ServiceStatus>(NDO_DATA_LASTSERVICECHECK,
      &ServiceStatus::last_check),
    KeyField<ServiceStatus>(NDO_DATA_LASTHARDSTATE,
      &ServiceStatus::last_hard_state),
    KeyField<ServiceStatus>(NDO_DATA_LASTHARDSTATECHANGE,
      &ServiceStatus::last_hard_state_change),
    KeyField<ServiceStatus>(NDO_DATA_LASTSERVICENOTIFICATION,
      &ServiceStatus::last_notification),
    KeyField<ServiceStatus>(NDO_DATA_LASTSTATECHANGE,
      &ServiceStatus::last_state_change),
    KeyField<ServiceStatus>(NDO_DATA_LASTTIMECRITICAL,
      &ServiceStatus::last_time_critical),
    KeyField<ServiceStatus>(NDO_DATA_LASTTIMEOK,
      &ServiceStatus::last_time_ok),
    KeyField<ServiceStatus>(NDO_DATA_LASTTIMEUNKNOWN,
      &ServiceStatus::last_time_unknown),
    KeyField<ServiceStatus>(NDO_DATA_LASTTIMEWARNING,
      &ServiceStatus::last_time_warning),
    KeyField<ServiceStatus>(NDO_DATA_LATENCY,
      &ServiceStatus::latency),
    KeyField<ServiceStatus>(NDO_DATA_LONGOUTPUT,
      &ServiceStatus::long_output),
    KeyField<ServiceStatus>(NDO_DATA_MAXCHECKATTEMPTS,
      &ServiceStatus::max_check_attempts),
    KeyField<ServiceStatus>(NDO_DATA_MODIFIEDSERVICEATTRIBUTES,
      &ServiceStatus::modified_attributes),
    KeyField<ServiceStatus>(NDO_DATA_NEXTSERVICECHECK,
      &ServiceStatus::next_check),
    KeyField<ServiceStatus>(NDO_DATA_NEXTSERVICENOTIFICATION,
      &ServiceStatus::next_notification),
    KeyField<ServiceStatus>(NDO_DATA_NOMORENOTIFICATIONS,
      &ServiceStatus::no_more_notifications),
    KeyField<ServiceStatus>(NDO_DATA_NORMALCHECKINTERVAL,
      &ServiceStatus::check_interval),
    KeyField<ServiceStatus>(NDO_DATA_OBSESSOVERSERVICE,
      &ServiceStatus::obsess_over),
    KeyField<ServiceStatus>(NDO_DATA_OUTPUT,
      &ServiceStatus::output),
    KeyField<ServiceStatus>(NDO_DATA_PASSIVESERVICECHECKSENABLED,
      &ServiceStatus::passive_checks_enabled),
    KeyField<ServiceStatus>(NDO_DATA_PERCENTSTATECHANGE,
      &ServiceStatus::percent_state_change),
    KeyField<ServiceStatus>(NDO_DATA_PERFDATA,
      &ServiceStatus::perf_data),
    KeyField<ServiceStatus>(NDO_DATA_PROBLEMHASBEENACKNOWLEDGED,
      &ServiceStatus::problem_has_been_acknowledged),
    KeyField<ServiceStatus>(NDO_DATA_PROCESSPERFORMANCEDATA,
      &ServiceStatus::process_performance_data),
    KeyField<ServiceStatus>(NDO_DATA_RETRYCHECKINTERVAL,
      &ServiceStatus::retry_interval),
    KeyField<ServiceStatus>(NDO_DATA_SCHEDULEDDOWNTIMEDEPTH,
      &ServiceStatus::scheduled_downtime_depth),
    KeyField<ServiceStatus>(NDO_DATA_SERVICE,
      &ServiceStatus::service),
    KeyField<ServiceStatus>(NDO_DATA_SERVICECHECKPERIOD,
      &ServiceStatus::check_period),
    KeyField<ServiceStatus>(NDO_DATA_SHOULDBESCHEDULED,
      &ServiceStatus::should_be_scheduled),
    KeyField<ServiceStatus>(NDO_DATA_STATETYPE,
      &ServiceStatus::state_type),
    KeyField<ServiceStatus>()
  };
