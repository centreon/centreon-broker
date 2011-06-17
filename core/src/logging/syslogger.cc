/*
** Copyright 2009-2011 Merethis
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

#include <syslog.h>
#include "com/centreon/broker/logging/syslogger.hh"

using namespace com::centreon::broker::logging;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Open syslog facility USER.
 */
syslogger::syslogger() {
  openlog("centreonbroker", 0, LOG_USER);
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Unused.
 */
syslogger::syslogger(syslogger const& s) : backend(s) {
  (void)s;
}

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
 *  Assignment operator.
 *
 *  @param[in] s Unused.
 *
 *  @return This object..
 */
syslogger& syslogger::operator=(syslogger const& s) {
  backend::operator=(s);
  return (*this);
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
                        level l) throw () {
  (void)len;
  (void)l;
  int priority;
  switch (log_type) {
   case CONFIG:
    priority = LOG_INFO;
    break ;
   case DEBUG:
    priority = LOG_DEBUG;
    break ;
   case ERROR:
    priority = LOG_ERR;
    break ;
   case INFO:
    priority = LOG_NOTICE;
    break ;
   default:
    priority = LOG_WARNING;
  }
  syslog(priority, "%s", msg);
  return ;
}
