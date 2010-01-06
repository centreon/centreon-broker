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

#include <stdlib.h>
#include <string.h>
#include "events/log.h"
#include "exception.h"
#include "interface/ndo/log.h"

/**
 *  Extract the first element of a log string.
 */
static char* log_extract_first(char* str, char** lasts)
{
  char* data;

  data = strtok_r(str, ";", lasts);
  if (!data)
    throw (Exception(0, "Log data extraction failed."));
  return (data);
}

/**
 *  Extract following elements of a log string.
 */
static char* log_extract(char** lasts)
{
  char* data;

  data = strtok_r(NULL, ";", lasts);
  if (!data)
    throw (Exception(0, "Log data extraction failed."));
  return (data);
}

/**
 *  Get the id of a log status.
 */
static int status_id(const char* status)
{
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
static int type_id(const char* type)
{
  int id;

  if (!strcmp(type, "HARD"))
    id = 1;
  else
    id = 0;
  return (id);
}

/**
 *  Extract NDO-formated log data to the C++ object.
 */
void Interface::NDO::SetLogData(Events::Log& log, const char* log_data)
{
  char* datadup;

  // Duplicate string so that we can split it with strtok_r.
  datadup = strdup(log_data);
  if (!datadup)
    throw (Exception(0, "Log data extraction failed."));

  try
    {
      char* lasts;

      // First part is the log description.
      lasts = datadup + strcspn(datadup, ":");
      if (*lasts)
        {
          *lasts = '\0';
          lasts = lasts + 1 + strspn(lasts + 1, " ");
        }
      if (!strcmp(datadup, "SERVICE ALERT"))
        {
          log.msg_type = 0;
          log.host = log_extract_first(lasts, &lasts);
          log.service = log_extract(&lasts);
          log.status = status_id(log_extract(&lasts));
          log.type = type_id(log_extract(&lasts));
          log.retry = strtol(log_extract(&lasts), NULL, 0);
          log.output = log_extract(&lasts);
        }
      else if (!strcmp(datadup, "HOST ALERT"))
        {
          log.msg_type = 1;
          log.host = log_extract_first(lasts, &lasts);
          log.status = status_id(log_extract(&lasts));
          log.type = type_id(log_extract(&lasts));
          log.retry = strtol(log_extract(&lasts), NULL, 0);
          log.output = log_extract(&lasts);
        }
      else if (!strcmp(datadup, "SERVICE NOTIFICATION"))
        {
          log.msg_type = 2;
          log.notification_contact = log_extract_first(lasts, &lasts);
          log.host = log_extract(&lasts);
          log.service = log_extract(&lasts);
          log.status = status_id(log_extract(&lasts));
          log.notification_cmd = log_extract(&lasts);
          log.output = log_extract(&lasts);
        }
      else if (!strcmp(datadup, "HOST NOTIFICATION"))
        {
          log.msg_type = 3;
          log.notification_contact = log_extract_first(lasts, &lasts);
          log.host = log_extract(&lasts);
          log.status = status_id(log_extract(&lasts));
          log.notification_cmd = log_extract(&lasts);
          log.output = log_extract(&lasts);
        }
      else if (!strcmp(datadup, "CURRENT SERVICE STATE"))
        {
          log.msg_type = 6;
          log.host = log_extract_first(lasts, &lasts);
          log.service = log_extract(&lasts);
          log.status = status_id(log_extract(&lasts));
          log.type = type_id(log_extract(&lasts));
        }
      else if (!strcmp(datadup, "CURRENT HOST STATE"))
        {
          log.msg_type = 7;
          log.host = log_extract_first(lasts, &lasts);
          log.status = status_id(log_extract(&lasts));
          log.type = type_id(log_extract(&lasts));
        }
      else if (!strcmp(datadup, "INITIAL HOST STATE"))
        {
          log.msg_type = 9;
          log.host = log_extract_first(lasts, &lasts);
          log.status = status_id(log_extract(&lasts));
          log.type = type_id(log_extract(&lasts));
        }
      else if (!strcmp(datadup, "INITIAL SERVICE STATE"))
        {
          log.msg_type = 8;
          log.host = log_extract_first(lasts, &lasts);
          log.service = log_extract(&lasts);
          log.status = status_id(log_extract(&lasts));
          log.type = type_id(log_extract(&lasts));
        }
      else if (!strcmp(datadup, "EXTERNAL_COMMAND"))
        {
          char* data;

          data = log_extract_first(lasts, &lasts);
          if (!strcmp(data, "ACKNOWLEDGE_SVC_PROBLEM"))
            {
              log.msg_type = 10;
              log.host = log_extract(&lasts);
              log.service = log_extract(&lasts);
              log.notification_contact = log_extract(&lasts);
              log.output = log_extract(&lasts);
            }
          else if (!strcmp(data, "ACKNOWLEDGE_HOST_PROBLEM"))
            {
              log.msg_type = 11;
              log.host = log_extract(&lasts);
              log.notification_contact = log_extract(&lasts);
              log.output = log_extract(&lasts);
            }
          else
            // XXX : seems like it should be something else ...
            throw (Exception(0, "Log data extraction failed."));
        }
      else if (!strcmp(datadup, "Warning"))
        {
          log.msg_type = 4;
          log.output = lasts;
        }
      else
        {
          log.msg_type = 5;
          log.output = log_data;
        }
    }
  catch (...) {}
  free(datadup);
  return ;
}
