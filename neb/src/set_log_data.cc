/*
** Copyright 2009-2011,2014-2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cstdlib>
#include <cstring>
#include "com/centreon/broker/neb/log_entry.hh"
#include "com/centreon/broker/exceptions/msg.hh"
#include "com/centreon/broker/neb/internal.hh"
#include "com/centreon/broker/neb/set_log_data.hh"

using namespace com::centreon::broker;

/**
 *  Extract the first element of a log string.
 */
static char* log_extract_first(char* str, char** lasts) {
  char* data(strtok_r(str, ";", lasts));
  if (!data)
    throw (exceptions::msg() << "log: data extraction failed");
  return (data);
}

/**
 *  Extract following elements of a log string.
 */
static char* log_extract(char** lasts) {
  char* data(strtok_r(NULL, ";", lasts));
  if (!data)
    throw (exceptions::msg() << "log: data extraction failed");
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
    throw (exceptions::msg() << "log: data extraction failed");

  try {
    char* lasts;

    // First part is the log description.
    lasts = datadup + strcspn(datadup, ":");
    if (*lasts) {
      *lasts = '\0';
      lasts = lasts + 1 + strspn(lasts + 1, " ");
    }
    if (!strcmp(datadup, "SERVICE ALERT")) {
      le.msg_type = 0;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), NULL, 10);
      le.output = log_extract(&lasts);
    }
    else if (!strcmp(datadup, "HOST ALERT")) {
      le.msg_type = 1;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), NULL, 10);
      le.output = log_extract(&lasts);
    }
    else if (!strcmp(datadup, "CURRENT SERVICE STATE")) {
      le.msg_type = 6;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), NULL, 10);
      le.output = log_extract(&lasts);
    }
    else if (!strcmp(datadup, "CURRENT HOST STATE")) {
      le.msg_type = 7;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), NULL, 10);
      le.output = log_extract(&lasts);
    }
    else if (!strcmp(datadup, "INITIAL HOST STATE")) {
      le.msg_type = 9;
      le.host_name = log_extract_first(lasts, &lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), NULL, 10);
      le.output = log_extract(&lasts);
    }
    else if (!strcmp(datadup, "INITIAL SERVICE STATE")) {
      le.msg_type = 8;
      le.host_name = log_extract_first(lasts, &lasts);
      le.service_description = log_extract(&lasts);
      le.status = status_id(log_extract(&lasts));
      le.log_type = type_id(log_extract(&lasts));
      le.retry = strtol(log_extract(&lasts), NULL, 10);
      le.output = log_extract(&lasts);
    }
    else if (!strcmp(datadup, "Warning")) {
      le.msg_type = 4;
      le.output = lasts;
    }
    else {
      le.msg_type = 5;
      le.output = log_data;
    }
  }
  catch (...) {}
  free(datadup);

  // Set host and service IDs.
  umap<std::string, int>::const_iterator host_it;
  std::map<std::pair<std::string, std::string>, std::pair<int, int> >::const_iterator service_it;
  host_it = neb::gl_hosts.find(le.host_name.toStdString());
  if (host_it != neb::gl_hosts.end())
    le.host_id = host_it->second;
  service_it = neb::gl_services.find(std::make_pair(le.host_name.toStdString(),
    le.service_description.toStdString()));
  if (service_it != neb::gl_services.end()) {
    le.host_id = service_it->second.first;
    le.service_id = service_it->second.second;
  }

  return ;
}
