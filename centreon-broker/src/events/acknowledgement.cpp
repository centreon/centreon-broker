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
#include "events/acknowledgement.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  This internal method is used to copy data defined inside the
 *  Acknowledgement class from an object to the current instance. This means
 *  that no superclass data are copied. This method is used in Acknowledgement
 *  copy constructor and in the = operator overload.
 *
 *  \see Acknowledgement(const Acknowledgement&)
 *  \see operator=
 *
 *  \param[in] ack Object to copy from.
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
 *  Acknowledgement default constructor. Set all members to their default value
 *  (0, NULL or equivalent).
 */
Acknowledgement::Acknowledgement()
{
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Acknowledgement copy constructor.
 *
 *  \param[in] ack Object to copy from.
 */
Acknowledgement::Acknowledgement(const Acknowledgement& ack) : Event(ack)
{
  this->InternalCopy(ack);
}

/**
 *  Acknowledgement destructor, release all acquired ressources.
 */
Acknowledgement::~Acknowledgement()
{
}

/**
 *  Overload of the = operator.
 *
 *  \param[in] ack Object to copy from.
 *
 *  \return *this
 */
Acknowledgement& Acknowledgement::operator=(const Acknowledgement& ack)
{
  this->Event::operator=(ack);
  this->InternalCopy(ack);
  return (*this);
}

/**
 *  Get the type of the acknowledgement.
 *
 *  \return The type of the acknowledgement (XXX : what are the types ?).
 */
short Acknowledgement::GetAcknowledgementType() const throw ()
{
  return (this->shorts_[ACKNOWLEDGEMENT_TYPE]);
}

/**
 *  Get the name of the acknowledgement author.
 *
 *  \return The name of the acknowledgement author.
 */
const std::string& Acknowledgement::GetAuthorName() const throw ()
{
  return (this->strings_[AUTHOR_NAME]);
}

/**
 *  Get the comment associated with the acknowledgement.
 *
 *  \return The comment associated with the acknowledgement.
 */
const std::string& Acknowledgement::GetComment() const throw ()
{
  return (this->strings_[COMMENT]);
}

/**
 *  XXX : need fix
 */
time_t Acknowledgement::GetEntryTime() const throw ()
{
  return (this->timets_[ENTRY_TIME]);
}

/**
 *  Get the name of the host associated with the acknowledgement.
 *
 *  \return The name of the host associated with the acknowledgement.
 */
const std::string& Acknowledgement::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Determines whether or not the acknowledgement is sticky.
 *
 *  \return 0 if the acknowledgement is not sticky.
 */
short Acknowledgement::GetIsSticky() const throw ()
{
  return (this->shorts_[IS_STICKY]);
}

/**
 *  Determines whether or not contacts should still be notified.
 *
 *  \return 0 if contacts shouldn't be notified.
 */
short Acknowledgement::GetNotifyContacts() const throw ()
{
  return (this->shorts_[NOTIFY_CONTACTS]);
}

/**
 *  XXX : fix
 */
short Acknowledgement::GetPersistentComment() const throw ()
{
  return (this->shorts_[PERSISTENT_COMMENT]);
}

/**
 *  Get the name of the service associated with the acknowledgement.
 *
 *  \return The name of the service associated with the acknowledgement.
 */
const std::string& Acknowledgement::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  XXX : need fix
 */
short Acknowledgement::GetState() const throw ()
{
  return (this->shorts_[STATE]);
}

/**
 *  Returns the type of this event (CentreonBroker::Event::ACKNOWLEDGEMENT).
 *
 *  \see CentreonBroker::Event
 *
 *  \return CentreonBroker::Event::ACKNOWLEDGEMENT
 */
int Acknowledgement::GetType() const throw ()
{
  return (Event::ACKNOWLEDGEMENT);
}

/**
 *  Set the type of the acknowledgement.
 *
 *  \see GetAcknowlegementType
 *
 *  \param[in] at The acknowledgement type (XXX : what are the available types
 *                ?).
 */
void Acknowledgement::SetAcknowledgementType(short at) throw ()
{
  this->shorts_[ACKNOWLEDGEMENT_TYPE] = at;
  return ;
}

/**
 *  Set the name of the acknowledgement author.
 *
 *  \see GetAuthorName
 *
 *  \param[in] an The name of the acknowledgement author.
 */
void Acknowledgement::SetAuthorName(const std::string& an)
{
  this->strings_[AUTHOR_NAME] = an;
  return ;
}

/**
 *  Set the comment associated with the acknowledgement.
 *
 *  \see GetComment
 *
 *  \param[in] c The comment associated with the acknowledgement.
 */
void Acknowledgement::SetComment(const std::string& c)
{
  this->strings_[COMMENT] = c;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetEntryTime
 */
void Acknowledgement::SetEntryTime(time_t et) throw ()
{
  this->timets_[ENTRY_TIME] = et;
  return ;
}

/**
 *  Set the name of the host associated with the acknowledgement.
 *
 *  \see GetHost
 *
 *  \param[in] h The name of the host associated with the acknowledgement.
 */
void Acknowledgement::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set whether or not the acknowledgement is sticky.
 *
 *  \see GetIsSticky
 *
 *  \param[in] is 0 if the acknowledgement isn't sticky.
 */
void Acknowledgement::SetIsSticky(short is) throw ()
{
  this->shorts_[IS_STICKY] = is;
  return ;
}

/**
 *  Set whether or not contacts should still be notified.
 *
 *  \see GetNotifyContacts
 *
 *  \param[in] nc 0 if contacts shouldn't be notified.
 */
void Acknowledgement::SetNotifyContacts(short nc) throw ()
{
  this->shorts_[NOTIFY_CONTACTS] = nc;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetPersistentComment
 */
void Acknowledgement::SetPersistentComment(short pc) throw ()
{
  this->shorts_[PERSISTENT_COMMENT] = pc;
  return ;
}

/**
 *  Set the name of the service associated with the acknowledgement.
 *
 *  \see GetService
 *
 *  \param[in] s The name of the service associated with the acknowledgement.
 */
void Acknowledgement::SetService(const std::string& s)
{
  this->strings_[SERVICE] = s;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetState
 */
void Acknowledgement::SetState(short s) throw ()
{
  this->shorts_[STATE] = s;
  return ;
}
