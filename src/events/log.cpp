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

#include <cstring>
#include "events/log.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Log::InternalCopy(const Log& log)
{
  memcpy(this->ints_, log.ints_, sizeof(this->ints_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = log.strings_[i];
  memcpy(this->timets_, log.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*          Public Methods             *
*                                     *
**************************************/

/**
 *  Log default constructor.
 */
Log::Log()
{
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Log copy constructor.
 */
Log::Log(const Log& log) : Event(log)
{
  this->InternalCopy(log);
}

/**
 *  Log destructor.
 */
Log::~Log() {}

/**
 *  Log operator= overload.
 */
Log& Log::operator=(const Log& log)
{
  this->Event::operator=(log);
  this->InternalCopy(log);
  return (*this);
}

/**
 *  Get the ctime member.
 */
time_t Log::GetCtime() const throw ()
{
  return (this->timets_[C_TIME]);
}

/**
 *  Get the host_name member.
 */
const std::string& Log::GetHostName() const throw ()
{
  return (this->strings_[HOST_NAME]);
}

/**
 *  Get the log_type member.
 */
const std::string& Log::GetLogType() const throw ()
{
  return (this->strings_[LOG_TYPE]);
}

/**
 *  Get the msg_type member.
 */
int Log::GetMsgType() const throw ()
{
  return (this->ints_[MSG_TYPE]);
}

/**
 *  Get the notification_cmd member.
 */
const std::string& Log::GetNotificationCmd() const throw ()
{
  return (this->strings_[NOTIFICATION_CMD]);
}

/**
 *  Get the notification_contact member.
 */
const std::string& Log::GetNotificationContact() const throw ()
{
  return (this->strings_[NOTIFICATION_CONTACT]);
}

/**
 *  Get the output member.
 */
const std::string& Log::GetOutput() const throw ()
{
  return (this->strings_[OUTPUT]);
}

/**
 *  Get the retry member.
 */
int Log::GetRetry() const throw ()
{
  return (this->ints_[RETRY]);
}

/**
 *  Get the service_description member.
 */
const std::string& Log::GetServiceDescription() const throw ()
{
  return (this->strings_[SERVICE_DESCRIPTION]);
}

/**
 *  Get the status member.
 */
const std::string& Log::GetStatus() const throw ()
{
  return (this->strings_[STATUS]);
}

/**
 *  Get the type member.
 */
int Log::GetType() const throw ()
{
  return (Event::LOG);
}

/**
 *  Set the ctime member.
 */
void Log::SetCtime(time_t c) throw ()
{
  this->timets_[C_TIME] = c;
  return ;
}

/**
 *  Set the host_name member.
 */
void Log::SetHostName(const std::string& hn)
{
  this->strings_[HOST_NAME] = hn;
  return ;
}

/**
 *  Set the type member.
 */
void Log::SetLogType(const std::string& lt)
{
  this->strings_[LOG_TYPE] = lt;
  return ;
}

/**
 *  Set the msg_type member.
 */
void Log::SetMsgType(int mt) throw ()
{
  this->ints_[MSG_TYPE] = mt;
  return ;
}

/**
 *  Set the notification_cmd member.
 */
void Log::SetNotificationCmd(const std::string& nc)
{
  this->strings_[NOTIFICATION_CMD] = nc;
  return ;
}

/**
 *  Set the notification_contact member.
 */
void Log::SetNotificationContact(const std::string& nc)
{
  this->strings_[NOTIFICATION_CONTACT] = nc;
  return ;
}

/**
 *  Set the output member.
 */
void Log::SetOutput(const std::string& o)
{
  this->strings_[OUTPUT] = o;
  return ;
}

/**
 *  Set the retry member.
 */
void Log::SetRetry(int r) throw ()
{
  this->ints_[RETRY] = r;
  return ;
}

/**
 *  Set the service_description member.
 */
void Log::SetServiceDescription(const std::string& sd)
{
  this->strings_[SERVICE_DESCRIPTION] = sd;
  return ;
}

/**
 *  Set the status member.
 */
void Log::SetStatus(const std::string& s)
{
  this->strings_[STATUS] = s;
  return ;
}
