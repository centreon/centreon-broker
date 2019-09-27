/*
** Copyright 2009-2011,2017 Centreon
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

#include "com/centreon/broker/logging/syslogger.hh"
#include <syslog.h>

using namespace com::centreon::broker::logging;

/**************************************
 *                                     *
 *           Public Methods            *
 *                                     *
 **************************************/

/**
 *  Constructor.
 *
 *  @param[in] facility Facility syslogger should write on.
 */
syslogger::syslogger(int facility) {
  openlog("centreonbroker", 0, facility);
}

/**
 *  Destructor.
 */
syslogger::~syslogger() {
  closelog();
}

/**
 *  Log message to syslog.
 *
 *  @param[in] msg      Message to log.
 *  @param[in] len      Length of msg.
 *  @param[in] log_type Type of logged message.
 *  @param[in] l        Priority of this message.
 */
void syslogger::log_msg(char const* msg,
                        unsigned int len,
                        type log_type,
                        level l) throw() {
  (void)len;
  (void)l;
  int priority;
  switch (log_type) {
    case config_type:
      priority = LOG_INFO;
      break;
    case debug_type:
      priority = LOG_DEBUG;
      break;
    case error_type:
      priority = LOG_ERR;
      break;
    case info_type:
    case perf_type:
      priority = LOG_NOTICE;
      break;
    default:
      priority = LOG_WARNING;
  }
  syslog(priority, "%s", msg);
  return;
}
