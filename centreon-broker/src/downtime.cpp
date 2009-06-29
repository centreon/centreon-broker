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
#include "downtime.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Downtime::InternalCopy(const Downtime& downtime)
{
  memcpy(this->ints_, downtime.ints_, sizeof(this->ints_));
  memcpy(this->shorts_, downtime.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = downtime.strings_[i];
  memcpy(this->timets_, downtime.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Downtime default constructor.
 */
Downtime::Downtime()
{
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Downtime copy constructor.
 */
Downtime::Downtime(const Downtime& downtime) : Event(downtime)
{
  this->InternalCopy(downtime);
}

/**
 *  Downtime destructor.
 */
Downtime::~Downtime()
{
}

/**
 *  Downtime operator= overload.
 */
Downtime& Downtime::operator=(const Downtime& downtime)
{
  this->Event::operator=(downtime);
  this->InternalCopy(downtime);
  return (*this);
}

/**
 *  Get the actual_end_time member.
 */
time_t Downtime::GetActualEndTime() const throw ()
{
  return (this->timets_[ACTUAL_END_TIME]);
}

/**
 *  Get the actual_end_time_usec member.
 */
int Downtime::GetActualEndTimeUsec() const throw ()
{
  return (this->ints_[ACTUAL_END_TIME_USEC]);
}

/**
 *  Get the actual_start_time member.
 */
time_t Downtime::GetActualStartTime() const throw ()
{
  return (this->timets_[ACTUAL_START_TIME]);
}

/**
 *  Get the actual_start_time_usec member.
 */
int Downtime::GetActualStartTimeUsec() const throw ()
{
  return (this->ints_[ACTUAL_START_TIME_USEC]);
}

/**
 *  Get the author_name member.
 */
const std::string& Downtime::GetAuthorName() const throw ()
{
  return (this->strings_[AUTHOR_NAME]);
}

/**
 *  Get the comment_data member.
 */
const std::string& Downtime::GetCommentData() const throw ()
{
  return (this->strings_[COMMENT_DATA]);
}

/**
 *  Get the downtime_type member.
 */
short Downtime::GetDowntimeType() const throw ()
{
  return (this->shorts_[DOWNTIME_TYPE]);
}

/**
 *  Get the duration member.
 */
short Downtime::GetDuration() const throw ()
{
  return (this->shorts_[DURATION]);
}

/**
 *  Get the entry_time member.
 */
time_t Downtime::GetEntryTime() const throw ()
{
  return (this->timets_[ENTRY_TIME]);
}

/**
 *  Get the host member.
 */
const std::string& Downtime::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Get the internal_id member.
 */
int Downtime::GetInternalId() const throw ()
{
  return (this->ints_[INTERNAL_ID]);
}

/**
 *  Get the is_fixed member.
 */
short Downtime::GetIsFixed() const throw ()
{
  return (this->shorts_[IS_FIXED]);
}

/**
 *  Get the scheduled_end_time member.
 */
time_t Downtime::GetScheduledEndTime() const throw ()
{
  return (this->timets_[SCHEDULED_END_TIME]);
}

/**
 *  Get the scheduled_start_time member.
 */
time_t Downtime::GetScheduledStartTime() const throw ()
{
  return (this->timets_[SCHEDULED_START_TIME]);
}

/**
 *  Get the service member.
 */
const std::string& Downtime::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  Get the triggered_by_id member.
 */
int Downtime::GetTriggeredById() const throw ()
{
  return (this->ints_[TRIGGERED_BY_ID]);
}

/**
 *  Get the type of the event.
 */
int Downtime::GetType() const throw ()
{
  return (Event::DOWNTIME);
}

/**
 *  Get the was_cancelled member.
 */
short Downtime::GetWasCancelled() const throw ()
{
  return (this->shorts_[WAS_CANCELLED]);
}

/**
 *  Get the was_started member.
 */
short Downtime::GetWasStarted() const throw ()
{
  return (this->shorts_[WAS_STARTED]);
}

/**
 *  Set the actual_end_time member.
 */
void Downtime::SetActualEndTime(time_t aet) throw ()
{
  this->timets_[ACTUAL_END_TIME] = aet;
  return ;
}

/**
 *  Set the actual_end_time_usec member.
 */
void Downtime::SetActualEndTimeUsec(int aetu) throw ()
{
  this->ints_[ACTUAL_END_TIME_USEC] = aetu;
  return ;
}

/**
 *  Set the actual_start_time member.
 */
void Downtime::SetActualStartTime(time_t ast) throw ()
{
  this->timets_[ACTUAL_START_TIME] = ast;
  return ;
}

/**
 *  Set the actual_start_time_usec member.
 */
void Downtime::SetActualStartTimeUsec(int astu) throw ()
{
  this->ints_[ACTUAL_START_TIME_USEC] = astu;
  return ;
}

/**
 *  Set the author_name member.
 */
void Downtime::SetAuthorName(const std::string& an)
{
  this->strings_[AUTHOR_NAME] = an;
  return ;
}

/**
 *  Set the comment_data member.
 */
void Downtime::SetCommentData(const std::string& cd)
{
  this->strings_[COMMENT_DATA] = cd;
  return ;
}

/**
 *  Set the downtime_type member.
 */
void Downtime::SetDowntimeType(short dt) throw ()
{
  this->shorts_[DOWNTIME_TYPE] = dt;
  return ;
}

/**
 *  Set the duration member.
 */
void Downtime::SetDuration(short d) throw ()
{
  this->shorts_[DURATION] = d;
  return ;
}

/**
 *  Set the entry_time member.
 */
void Downtime::SetEntryTime(time_t et) throw ()
{
  this->timets_[ENTRY_TIME] = et;
  return ;
}

/**
 *  Set the host member.
 */
void Downtime::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set the internal_id member.
 */
void Downtime::SetInternalId(int ii) throw ()
{
  this->ints_[INTERNAL_ID] = ii;
  return ;
}

/**
 *  Set the is_fixed member.
 */
void Downtime::SetIsFixed(int i_f) throw ()
{
  this->ints_[IS_FIXED] = i_f;
  return ;
}

/**
 *  Set the scheduled_end_time member.
 */
void Downtime::SetScheduledEndTime(time_t set) throw ()
{
  this->timets_[SCHEDULED_END_TIME] = set;
  return ;
}

/**
 *  Set the scheduled_start_time member.
 */
void Downtime::SetScheduledStartTime(time_t sst) throw ()
{
  this->timets_[SCHEDULED_START_TIME] = sst;
  return ;
}

/**
 *  Set the service member.
 */
void Downtime::SetService(const std::string& service)
{
  this->strings_[SERVICE] = service;
  return ;
}

/**
 *  Set the triggered_by_id member.
 */
void Downtime::SetTriggeredById(int tbi) throw ()
{
  this->ints_[TRIGGERED_BY_ID] = tbi;
  return ;
}

/**
 *  Set the was_cancelled member.
 */
void Downtime::SetWasCancelled(short wc) throw ()
{
  this->shorts_[WAS_CANCELLED] = wc;
  return ;
}

/**
 *  Set the was_started member.
 */
void Downtime::SetWasStarted(short ws) throw ()
{
  this->shorts_[WAS_STARTED] = ws;
  return ;
}
