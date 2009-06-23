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
#include "acknowledgement.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Acknowledgement::InternalCopy(const Acknowledgement& ack)
{
  memcpy(this->shorts_, ack.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = ack.strings_[i];
  memcpy(this->timets_, ack.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Acknowledgement default constructor.
 */
Acknowledgement::Acknowledgement()
{
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Acknowledgement copy constructor.
 */
Acknowledgement::Acknowledgement(const Acknowledgement& ack) : Event(ack)
{
  this->InternalCopy(ack);
}

/**
 *  Acknowledgement destructor.
 */
Acknowledgement::~Acknowledgement()
{
}

/**
 *  Acknowledgement operator= overload.
 */
Acknowledgement& Acknowledgement::operator=(const Acknowledgement& ack)
{
  this->Event::operator=(ack);
  this->InternalCopy(ack);
  return (*this);
}

/**
 *  Get the acknowledgement_type member.
 */
short Acknowledgement::GetAcknowledgementType() const throw ()
{
  return (this->shorts_[ACKNOWLEDGEMENT_TYPE]);
}

/**
 *  Get the author_name member.
 */
const std::string& Acknowledgement::GetAuthorName() const throw ()
{
  return (this->strings_[AUTHOR_NAME]);
}

/**
 *  Get the comment member.
 */
const std::string& Acknowledgement::GetComment() const throw ()
{
  return (this->strings_[COMMENT]);
}

/**
 *  Get the entry_time member.
 */
time_t Acknowledgement::GetEntryTime() const throw ()
{
  return (this->timets_[ENTRY_TIME]);
}

/**
 *  Get the host member.
 */
const std::string& Acknowledgement::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Get the is_sticky member.
 */
short Acknowledgement::GetIsSticky() const throw ()
{
  return (this->shorts_[IS_STICKY]);
}

/**
 *  Get the notify_contacts member.
 */
short Acknowledgement::GetNotifyContacts() const throw ()
{
  return (this->shorts_[NOTIFY_CONTACTS]);
}

/**
 *  Get the persistent_comment member.
 */
short Acknowledgement::GetPersistentComment() const throw ()
{
  return (this->shorts_[PERSISTENT_COMMENT]);
}

/**
 *  Get the service type.
 */
const std::string& Acknowledgement::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  Get the state type.
 */
short Acknowledgement::GetState() const throw ()
{
  return (this->shorts_[STATE]);
}

/**
 *  Return the event type.
 */
int Acknowledgement::GetType() const throw ()
{
  return (Event::ACKNOWLEDGEMENT);
}

/**
 *  Set the acknowledgement_type member.
 */
void Acknowledgement::SetAcknowledgementType(short at) throw ()
{
  this->shorts_[ACKNOWLEDGEMENT_TYPE] = at;
  return ;
}

/**
 *  Set the author_name member.
 */
void Acknowledgement::SetAuthorName(const std::string& an)
{
  this->strings_[AUTHOR_NAME] = an;
  return ;
}

/**
 *  Set the comment member.
 */
void Acknowledgement::SetComment(const std::string& c)
{
  this->strings_[COMMENT] = c;
  return ;
}

/**
 *  Set the entry_time member.
 */
void Acknowledgement::SetEntryTime(time_t et) throw ()
{
  this->timets_[ENTRY_TIME] = et;
  return ;
}

/**
 *  Set the host member.
 */
void Acknowledgement::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set the is_sticky member.
 */
void Acknowledgement::SetIsSticky(short is) throw ()
{
  this->shorts_[IS_STICKY] = is;
  return ;
}

/**
 *  Set the notify_contacts member.
 */
void Acknowledgement::SetNotifyContacts(short nc) throw ()
{
  this->shorts_[NOTIFY_CONTACTS] = nc;
  return ;
}

/**
 *  Set the persistent_comment member.
 */
void Acknowledgement::SetPersistentComment(short pc) throw ()
{
  this->shorts_[PERSISTENT_COMMENT] = pc;
  return ;
}

/**
 *  Set the service member.
 */
void Acknowledgement::SetService(const std::string& s)
{
  this->strings_[SERVICE] = s;
  return ;
}

/**
 *  Set the state member.
 */
void Acknowledgement::SetState(short s) throw ()
{
  this->shorts_[STATE] = s;
  return ;
}
