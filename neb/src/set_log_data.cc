/*
** Copyright 2009-2011,2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/neb/set_log_data.hh"
#include <cstdlib>
#include <cstring>
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/engine/host.hh"
#include "com/centreon/engine/service.hh"

using namespace com::centreon::broker;

/**
 *  Extract the first element of a log string.
 */
static char* log_extract_first(char* str, char** lasts) {
  char* data(strtok_r(str, ";", lasts));
  if (!data)
    throw(exceptions::msg() << "log: data extraction failed");
  return (data);
}

/**
 *  Extract following elements of a log string.
 */
static char* log_extract(char** lasts) {
  char* data(strtok_r(nullptr, ";", lasts));
  if (!data)
    throw(exceptions::msg() << "log: data extraction failed");
  return (data);
}

/**
 *  Get the id of a log status.
 */
static int status_id(char const* status) {
  int id;
  if (!strcmp(status, "DOWN") || !strcmp(status, "WARNING"))
    id = 1;
  else if (!strcmp(status, "UNREACHABLE") || !strcmp(status, "CRITICAL"))
    id = 2;
  else if (!strcmp(status, "UNKNOWN"))
    id = 3;
  else if (!strcmp(status, "PENDING"))
    id = 4;
  else
    id = 0;
  return (id);
}

/**
 *  Get the notification status of a log.
 */
static int notification_status_id(char const* status) {
  char const* ptr(strchr(status, '('));
  int id;
  if (ptr) {
    std::string substatus(ptr + 1);
    size_t it(substatus.find_first_of(')'));
    if (it != std::string::npos)
      substatus.erase(it);
    id = status_id(substatus.c_str());
  } else
    id = status_id(status);
  return (id);
}

/**
 *  Get the id of a log type.
 */
static int type_id(char const* type) {
  int id;
  if (!strcmp(type, "HARD"))
    id = 1;
  else
    id = 0;
  return (id);
}

/**
 *  Extract Nagios-formated log data to the C++ object.
 */
void neb::set_log_data(neb::log_entry& le, char const* log_data) {
  // Duplicate string so that we can split it with strtok_r.
  char* datadup(strdup(log_data));
  if (!datadup)
    throw(exceptions::msg() << "log: data extraction failed");

  try {
    char* lasts;

    // First part is the log description.
    lasts = datadup + strcspn(datadup, ":");
    if (*lasts) {
      *lasts = '\0';
      lasts = lasts + 1 + strspn(lasts + 1, " ");
    }
    if (!strcmp(datadup, "SERVICE ALERT")) {
      le.msg_type = log_entry::service_alert;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "HOST ALERT")) {
      le.msg_type = log_entry::host_alert;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "SERVICE NOTIFICATION")) {
      le.msg_type = log_entry::service_notification;
      le.notification_contact = log_extract_first(lasts, &lasts);
      le.host_name = log_extract(&lasts);
      le.service_description = log_extract(&lasts);
      le.status = notification_status_id(log_extract(&lasts));
      le.notification_cmd = log_extract(&lasts);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "HOST NOTIFICATION")) {
      le.msg_type = log_entry::host_notification;
      le.notification_contact = log_extract_first(lasts, &lasts);
      le.host_name = log_extract(&lasts);
      le.status = notification_status_id(log_extract(&lasts));
      le.notification_cmd = log_extract(&lasts);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "INITIAL HOST STATE")) {
      le.msg_type = log_entry::host_initial_state;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "INITIAL SERVICE STATE")) {
      le.msg_type = log_entry::service_initial_state;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "EXTERNAL COMMAND")) {
      char* data(log_extract_first(lasts, &lasts));
      if (!strcmp(data, "ACKNOWLEDGE_SVC_PROBLEM")) {
        le.msg_type = log_entry::service_acknowledge_problem;
        le.host_name = log_extract(&lasts);
        le.service_description = log_extract(&lasts);
        log_extract(&lasts);
        log_extract(&lasts);
        log_extract(&lasts);
        le.notification_contact = log_extract(&lasts);
        le.output = log_extract(&lasts);
      } else if (!strcmp(data, "ACKNOWLEDGE_HOST_PROBLEM")) {
        le.msg_type = log_entry::host_acknowledge_problem;
        le.host_name = log_extract(&lasts);
        log_extract(&lasts);
        log_extract(&lasts);
        log_extract(&lasts);
        le.notification_contact = log_extract(&lasts);
        le.output = log_extract(&lasts);
      } else {
        le.msg_type = log_entry::other;
        le.output = log_data;
      }
    }  else if (!strcmp(datadup, "HOST EVENT HANDLER")) {
      le.msg_type = log_entry::host_event_handler;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "SERVICE EVENT HANDLER")) {
      le.msg_type = log_entry::service_event_handler;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "GLOBAL HOST EVENT HANDLER")) {
      le.msg_type = log_entry::global_host_event_handler;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "GLOBAL SERVICE EVENT HANDLER")) {
      le.msg_type = log_entry::global_service_event_handler;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), nullptr, 10);
      le.output = log_extract(&lasts);
    } else if (!strcmp(datadup, "Warning")) {
      le.msg_type = log_entry::warning;
      le.output = lasts;
    } else {
      le.msg_type = log_entry::other;
      le.output = log_data;
    }
  } catch (...) {
  }
  free(datadup);

  // Set host and service IDs.
  le.host_id = engine::get_host_id(le.host_name);
  le.service_id = engine::get_service_id(le.host_name, le.service_description);
}
