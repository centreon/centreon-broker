/*
** Copyright 2009-2011 Centreon
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

#ifndef CCB_LOGGING_SYSLOGGER_HH_
#define CCB_LOGGING_SYSLOGGER_HH_

#include <syslog.h>
#include "com/centreon/broker/logging/backend.hh"
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace logging {
/**
 *  @class syslogger syslogger.hh "com/centreon/broker/logging/syslogger.hh"
 *  @brief Log messages to syslog.
 *
 *  Log messages to a configured facility of syslog.
 */
class syslogger : public backend {
 public:
  syslogger(int facility = LOG_LOCAL0);
  syslogger(syslogger const& s);
  ~syslogger();
  syslogger& operator=(syslogger const& s);
  void log_msg(char const* msg,
               unsigned int len,
               type log_type,
               level l) throw();
};
}  // namespace logging

CCB_END()

#endif /* !CCB_LOGGING_SYSLOGGER_HH_ */
