/*
**  Copyright 2010 MERETHIS
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

#include <syslog.h>
#include "logging/syslogger.hh"

using namespace logging;

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
syslogger::syslogger()
{
  openlog("centreonbroker", 0, LOG_USER);
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Unused.
 */
syslogger::syslogger(syslogger const& s) : backend(s)
{
  (void)s;
}

/**
 *  Constructor.
 *
 *  @param[in] facility Facility syslogger should write on.
 */
syslogger::syslogger(int facility)
{
  openlog("centreonbroker", 0, facility);
}

/**
 *  Destructor.
 */
syslogger::~syslogger()
{
  closelog();
}

/**
 *  Assignment operator overload.
 *
 *  @param[in] s Unused.
 *
 *  @return Current instance.
 */
syslogger& syslogger::operator=(syslogger const& s)
{
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
                        level l) throw ()
{
  int priority;

  (void)len;
  (void)l;
  switch (log_type)
    {
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
