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
#include "events/comment.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  This internal method is used to copy data defined inside the Comment class
 *  from an object to the current instance. This means that no superclass data
 *  are copied. This method is used in Comment copy constructor and in the =
 *  operator overload.
 *
 *  \see Comment(const Comment&)
 *  \see operator=
 *
 *  \param[in] comment Object to copy from.
 */
void Comment::InternalCopy(const Comment& comment)
{
  memcpy(this->bools_, comment.bools_, sizeof(this->bools_));
  memcpy(this->shorts_, comment.shorts_, sizeof(this->shorts_));
  for (unsigned int i = 0; i < STRING_NB; i++)
    this->strings_[i] = comment.strings_[i];
  memcpy(this->timets_, comment.timets_, sizeof(this->timets_));
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Comment default constructor. Set all members to their default value (0,
 *  NULL or equivalent).
 */
Comment::Comment()
{
  memset(this->bools_, 0, sizeof(this->bools_));
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Comment copy constructor.
 *
 *  \param[in] comment Object to copy from.
 */
Comment::Comment(const Comment& comment) : Event(comment)
{
  this->InternalCopy(comment);
}

/**
 *  Comment destructor, release all acquired ressources.
 */
Comment::~Comment()
{
}

/**
 *  Overload of the = operator.
 *
 *  \param[in] comment Object to copy from.
 *
 *  \return *this
 */
Comment& Comment::operator=(const Comment& comment)
{
  this->Event::operator=(comment);
  this->InternalCopy(comment);
  return (*this);
}

/**
 *  Get the name of the comment author.
 *
 *  \return The name of the comment author.
 */
const std::string& Comment::GetAuthorName() const throw ()
{
  return (this->strings_[AUTHOR_NAME]);
}

/**
 *  Get the comment content.
 *
 *  \return The comment content.
 */
const std::string& Comment::GetCommentData() const throw ()
{
  return (this->strings_[COMMENT_DATA]);
}

/**
 *  Get the date on which the comment was made.
 *
 *  \return The date on which the comment was made.
 */
time_t Comment::GetCommentTime() const throw ()
{
  return (this->timets_[COMMENT_TIME]);
}

/**
 *  Get the type of the comment (XXX : which types ?).
 *
 *  \return The type of the comment.
 */
short Comment::GetCommentType() const throw ()
{
  return (this->shorts_[COMMENT_TYPE]);
}

/**
 *  Get the time on which the comment was deleted.
 *
 *  \return The time on which the comment was deleted.
 */
time_t Comment::GetDeletionTime() const throw ()
{
  return (this->timets_[DELETION_TIME]);
}

/**
 *  XXX : need fix
 */
time_t Comment::GetEntryTime() const throw ()
{
  return (this->timets_[ENTRY_TIME]);
}

/**
 *  XXX : need fix
 */
short Comment::GetEntryType() const throw ()
{
  return (this->shorts_[ENTRY_TYPE]);
}

/**
 *  Get the date on which the comment expires.
 *
 *  \return The date on which the comment expires.
 */
time_t Comment::GetExpireTime() const throw ()
{
  return (this->timets_[EXPIRE_TIME]);
}

/**
 *  Determines whether or not the comment expires.
 *
 *  \return true if the comment expires, false otherwise.
 */
bool Comment::GetExpires() const throw ()
{
  return (this->bools_[EXPIRES]);
}

/**
 *  Get the name of the host associated with the comment.
 *
 *  \return The name of the host associated with the comment.
 */
const std::string& Comment::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Determines whether or not the comment is persistent.
 *
 *  \return true if the comment is persistent, false otherwise.
 */
bool Comment::GetPersistent() const throw ()
{
  return (this->bools_[PERSISTENT]);
}

/**
 *  Get the name of the service associated with the comment.
 *
 *  \return The name of the service associated with the comment.
 */
const std::string& Comment::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  Get the comment source.
 *
 *  \return The comment source (XXX : which sources ?).
 */
short Comment::GetSource() const throw ()
{
  return (this->shorts_[SOURCE]);
}

/**
 *  Returns the type of this event (CentreonBroker::Event::COMMENT).
 *
 *  \see CentreonBroker::Event
 *
 *  \return CentreonBroker::Event::COMMENT
 */
int Comment::GetType() const throw ()
{
  return (Event::COMMENT);
}

/**
 *  Set the name of the comment author.
 *
 *  \see GetAuthorName
 *
 *  \param[in] an The name of the comment author.
 */
void Comment::SetAuthorName(const std::string& an)
{
  this->strings_[AUTHOR_NAME] = an;
  return ;
}

/**
 *  Set the comment content.
 *
 *  \see GetCommentData
 *
 *  \param[in] cd The comment content.
 */
void Comment::SetCommentData(const std::string& cd)
{
  this->strings_[COMMENT_DATA] = cd;
  return ;
}

/**
 *  Set the time on which the comment was made.
 *
 *  \see GetCommentTime
 *
 *  \param[in] ct The time on which the comment was made.
 */
void Comment::SetCommentTime(time_t ct) throw ()
{
  this->timets_[COMMENT_TIME] = ct;
  return ;
}

/**
 *  Set the type of the comment (XXX : what are the available types ?).
 *
 *  \see GetCommentType
 *
 *  \param[in] ct The type of the comment.
 */
void Comment::SetCommentType(short ct) throw ()
{
  this->shorts_[COMMENT_TYPE] = ct;
  return ;
}

/**
 *  Set the date on which the comment was deleted.
 *
 *  \see GetDeletionTime
 *
 *  \param[in] dt The date on which the comment was deleted.
 */
void Comment::SetDeletionTime(time_t dt) throw ()
{
  this->timets_[DELETION_TIME] = dt;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetEntryTime
 */
void Comment::SetEntryTime(time_t et) throw ()
{
  this->timets_[ENTRY_TIME] = et;
  return ;
}

/**
 *  XXX : need fix
 *
 *  \see GetEntryType
 */
void Comment::SetEntryType(short et) throw ()
{
  this->shorts_[ENTRY_TYPE] = et;
  return ;
}

/**
 *  Set the date on which the comment expires.
 *
 *  \see GetExpireTime
 *
 *  \param[in] et The date on which the comment expires.
 */
void Comment::SetExpireTime(time_t et) throw ()
{
  this->timets_[EXPIRE_TIME] = et;
  return ;
}

/**
 *  Set whether or not the comment expires.
 *
 *  \see GetExpires
 *
 *  \param[in] e true if the comment does expire, false otherwise.
 */
void Comment::SetExpires(bool e) throw ()
{
  this->bools_[EXPIRES] = e;
  return ;
}

/**
 *  Set the name of the host associated with the comment.
 *
 *  \see GetHost
 *
 *  \param[in] h The name of the host associated with the comment.
 */
void Comment::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set whether or not the comment is persistent.
 *
 *  \see GetPersistent
 *
 *  \param[in] ip true if the comment is persistent, false otherwise.
 */
void Comment::SetPersistent(bool p) throw ()
{
  this->bools_[PERSISTENT] = p;
  return ;
}

/**
 *  Set the name of the service associated with the comment.
 *
 *  \see GetService
 *
 *  \param[in] s The name of the service associated with the comment.
 */
void Comment::SetService(const std::string& s)
{
  this->strings_[SERVICE] = s;
  return ;
}

/**
 *  Set the comment source (XXX : what are the available sources ?).
 *
 *  \see GetSource
 *
 *  \return The comment source.
 */
void Comment::SetSource(short cs) throw ()
{
  this->shorts_[SOURCE] = cs;
  return ;
}
