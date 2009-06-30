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
#include "events/downtime.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  This internal method is used to copy data defined inside the Downtime class
 *  from an object to the current instance. This means that no superclass data
 *  are copied. This method is used in Downtime copy constructor and in the =
 *  operator overload.
 *
 *  \see Downtime(const Downtime&)
 *  \see operator=
 *
 *  \param[in] downtime Object to copy from.
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
 *  Downtime default constructor. Set all members to their default value (0,
 *  NULL or equivalent).
 */
Downtime::Downtime()
{
  memset(this->ints_, 0, sizeof(this->ints_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Downtime copy constructor.
 *
 *  \param[in] downtime Object to copy from.
 */
Downtime::Downtime(const Downtime& downtime) : Event(downtime)
{
  this->InternalCopy(downtime);
}

/**
 *  Downtime destructor, release all acquired ressources.
 */
Downtime::~Downtime()
{
}

/**
 *  Overload of the = operator.
 *
 *  \param[in] downtime Object to copy from.
 *
 *  \return *this
 */
Downtime& Downtime::operator=(const Downtime& downtime)
{
  this->Event::operator=(downtime);
  this->InternalCopy(downtime);
  return (*this);
}

/**
 *  XXX : need fix
 */
time_t Downtime::GetActualEndTime() const throw ()
{
  return (this->timets_[ACTUAL_END_TIME]);
}

/**
 *  XXX : need fix
 */
time_t Downtime::GetActualStartTime() const throw ()
{
  return (this->timets_[ACTUAL_START_TIME]);
}

/**
 *  Get the name of the user who defined the downtime.
 *
 *  \return The name of the user who defined the downtime.
 */
const std::string& Downtime::GetAuthorName() const throw ()
{
  return (this->strings_[AUTHOR_NAME]);
}

/**
 *  Get the comment associated with the downtime.
 *
 *  \return The comment associated with the downtime.
 */
const std::string& Downtime::GetCommentData() const throw ()
{
  return (this->strings_[COMMENT_DATA]);
}

/**
 *  Get the type of the downtime (XXX : what are the available types ?).
 *
 *  \return The type of the downtime.
 */
short Downtime::GetDowntimeType() const throw ()
{
  return (this->shorts_[DOWNTIME_TYPE]);
}

/**
 *  Get the duration of the downtime.
 *
 *  \return The duration of the downtime.
 */
short Downtime::GetDuration() const throw ()
{
  return (this->shorts_[DURATION]);
}

/**
 *  XXX : need fix
 */
time_t Downtime::GetEntryTime() const throw ()
{
  return (this->timets_[ENTRY_TIME]);
}

/**
 *  Get the name of the host associated with the downtime.
 *
 *  \return The name of the host associated with the downtime.
 */
const std::string& Downtime::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  XXX : need fix
 */
int Downtime::GetInternalId() const throw ()
{
  return (this->ints_[INTERNAL_ID]);
}

/**
 *  Determines whether or not the downtime is fixed.
 *
 *  \return 0 if the downtime isn't fixed.
 */
short Downtime::GetIsFixed() const throw ()
{
  return (this->shorts_[IS_FIXED]);
}

/**
 *  XXX : need fix
 */
time_t Downtime::GetScheduledEndTime() const throw ()
{
  return (this->timets_[SCHEDULED_END_TIME]);
}

/**
 *  XXX : need fix
 */
time_t Downtime::GetScheduledStartTime() const throw ()
{
  return (this->timets_[SCHEDULED_START_TIME]);
}

/**
 *  Get the name of the service associated with the downtime.
 *
 *  \return The name of the service associated with the downtime.
 */
const std::string& Downtime::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  XXX : need fix
 */
int Downtime::GetTriggeredById() const throw ()
{
  return (this->ints_[TRIGGERED_BY_ID]);
}

/**
 *  Returns the type of this event (Event::DOWNTIME).
 *
 *  \return Event::DOWNTIME
 */
int Downtime::GetType() const throw ()
{
  return (Event::DOWNTIME);
}

/**
 *  Determines whether or not the downtime was cancelled.
 *
 *  \return 0 if the downtime wasn't cancelled.
 */
short Downtime::GetWasCancelled() const throw ()
{
  return (this->shorts_[WAS_CANCELLED]);
}

/**
 *  Determines whether or not the downtime was started.
 *
 *  \return 0 if the downtime wasn't started.
 */
short Downtime::GetWasStarted() const throw ()
{
  return (this->shorts_[WAS_STARTED]);
}

/**
 *  XXX : need fix
 *
 *  \see GetActualEndTime
 */
void Downtime::SetActualEndTime(time_t aet) throw ()
{
  this->timets_[ACTUAL_END_TIME] = aet;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetActualStartTime
 */
void Downtime::SetActualStartTime(time_t ast) throw ()
{
  this->timets_[ACTUAL_START_TIME] = ast;
  return ;
}

/**
 *  Set the name of the user who defined the downtime.
 *
 *  \see GetAuthorName
 *
 *  \param[in] an The name of the user who defined the downtime.
 */
void Downtime::SetAuthorName(const std::string& an)
{
  this->strings_[AUTHOR_NAME] = an;
  return ;
}

/**
 *  Set the comment associated with the downtime.
 *
 *  \see GetCommentData
 *
 *  \param[in] cd The comment associated with the downtime.
 */
void Downtime::SetCommentData(const std::string& cd)
{
  this->strings_[COMMENT_DATA] = cd;
  return ;
}

/**
 *  Set the type of the downtime (XXX : what are the available types ?).
 *
 *  \see GetDowntimeType
 *
 *  \param[in] dt The type of the downtime.
 */
void Downtime::SetDowntimeType(short dt) throw ()
{
  this->shorts_[DOWNTIME_TYPE] = dt;
  return ;
}

/**
 *  Set the duration of the downtime.
 *
 *  \see GetDuration
 *
 *  \param[in] d The duration of the downtime.
 */
void Downtime::SetDuration(short d) throw ()
{
  this->shorts_[DURATION] = d;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetEntryTime
 */
void Downtime::SetEntryTime(time_t et) throw ()
{
  this->timets_[ENTRY_TIME] = et;
  return ;
}

/**
 *  Set the name of the host associated with the downtime.
 *
 *  \see GetHost
 *
 *  \param[in] h The name of the host associated with the downtime.
 */
void Downtime::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetInternalId
 */
void Downtime::SetInternalId(int ii) throw ()
{
  this->ints_[INTERNAL_ID] = ii;
  return ;
}

/**
 *  Set whether or not the downtime is fixed.
 *
 *  \see GetIsFixed
 *
 *  \param[in] i_f 0 if the downtime isn't fixed.
 */
void Downtime::SetIsFixed(int i_f) throw ()
{
  this->ints_[IS_FIXED] = i_f;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetScheduledEndTime
 */
void Downtime::SetScheduledEndTime(time_t set) throw ()
{
  this->timets_[SCHEDULED_END_TIME] = set;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetScheduledStartTime
 */
void Downtime::SetScheduledStartTime(time_t sst) throw ()
{
  this->timets_[SCHEDULED_START_TIME] = sst;
  return ;
}

/**
 *  Set the name of the service associated with the downtime.
 *
 *  \see GetService
 *
 *  \param[in] service The name of the service associated with the downtime.
 */
void Downtime::SetService(const std::string& service)
{
  this->strings_[SERVICE] = service;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetTriggeredById
 */
void Downtime::SetTriggeredById(int tbi) throw ()
{
  this->ints_[TRIGGERED_BY_ID] = tbi;
  return ;
}

/**
 *  Set whether or not the downtime was cancelled.
 *
 *  \see GetWasCancelled
 *
 *  \param[in] wc 0 if the downtime wasn't cancelled.
 */
void Downtime::SetWasCancelled(short wc) throw ()
{
  this->shorts_[WAS_CANCELLED] = wc;
  return ;
}

/**
 *  Set whether or not the downtime was started.
 *
 *  \see GetWasStarted
 *
 *  \param[in] ws 0 if the downtime wasn't started.
 */
void Downtime::SetWasStarted(short ws) throw ()
{
  this->shorts_[WAS_STARTED] = ws;
  return ;
}
