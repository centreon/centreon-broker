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
#include "comment.h"

using namespace CentreonBroker;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy all internal data of the given object to the current instance.
 */
void Comment::InternalCopy(const Comment& comment)
{
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
 *  Comment default constructor.
 */
Comment::Comment()
{
  memset(this->shorts_, 0, sizeof(this->shorts_));
  memset(this->timets_, 0, sizeof(this->timets_));
}

/**
 *  Comment copy constructor.
 */
Comment::Comment(const Comment& comment) : Event(comment)
{
  this->InternalCopy(comment);
}

/**
 *  Comment destructor.
 */
Comment::~Comment()
{
}

/**
 *  Comment operator= overload.
 */
Comment& Comment::operator=(const Comment& comment)
{
  this->Event::operator=(comment);
  this->InternalCopy(comment);
  return (*this);
}

/**
 *  Get the author_name member.
 */
const std::string& Comment::GetAuthorName() const throw ()
{
  return (this->strings_[AUTHOR_NAME]);
}

/**
 *  Get the comment_data member.
 */
const std::string& Comment::GetCommentData() const throw ()
{
  return (this->strings_[COMMENT_DATA]);
}

/**
 *  Get the comment_source member.
 */
short Comment::GetCommentSource() const throw ()
{
  return (this->shorts_[COMMENT_SOURCE]);
}

/**
 *  Get the comment_time member.
 */
time_t Comment::GetCommentTime() const throw ()
{
  return (this->timets_[COMMENT_TIME]);
}

/**
 *  Get the comment_type member.
 */
short Comment::GetCommentType() const throw ()
{
  return (this->shorts_[COMMENT_TYPE]);
}

/**
 *  Get the deletion_time member.
 */
time_t Comment::GetDeletionTime() const throw ()
{
  return (this->timets_[DELETION_TIME]);
}

/**
 *  Get the entry_time member.
 */
time_t Comment::GetEntryTime() const throw ()
{
  return (this->timets_[ENTRY_TIME]);
}

/**
 *  Get the entry_type member.
 */
short Comment::GetEntryType() const throw ()
{
  return (this->shorts_[ENTRY_TYPE]);
}

/**
 *  Get the expiration_time member.
 */
time_t Comment::GetExpirationTime() const throw ()
{
  return (this->timets_[EXPIRATION_TIME]);
}

/**
 *  Get the expires member.
 */
short Comment::GetExpires() const throw ()
{
  return (this->shorts_[EXPIRES]);
}

/**
 *  Get the host member.
 */
const std::string& Comment::GetHost() const throw ()
{
  return (this->strings_[HOST]);
}

/**
 *  Get the is_persistent member.
 */
short Comment::GetIsPersistent() const throw ()
{
  return (this->shorts_[IS_PERSISTENT]);
}

/**
 *  Get the service member.
 */
const std::string& Comment::GetService() const throw ()
{
  return (this->strings_[SERVICE]);
}

/**
 *  Get the type of the event.
 */
int Comment::GetType() const throw ()
{
  return (Event::COMMENT);
}

/**
 *  Set the author_name member.
 */
void Comment::SetAuthorName(const std::string& an)
{
  this->strings_[AUTHOR_NAME] = an;
  return ;
}

/**
 *  Set the comment_data member.
 */
void Comment::SetCommentData(const std::string& cd)
{
  this->strings_[COMMENT_DATA] = cd;
  return ;
}

/**
 *  Set the comment_source member.
 */
void Comment::SetCommentSource(short cs) throw ()
{
  this->shorts_[COMMENT_SOURCE] = cs;
  return ;
}

/**
 *  Set the comment_time member.
 */
void Comment::SetCommentTime(time_t ct) throw ()
{
  this->timets_[COMMENT_TIME] = ct;
  return ;
}

/**
 *  Set the comment_type member.
 */
void Comment::SetCommentType(short ct) throw ()
{
  this->shorts_[COMMENT_TYPE] = ct;
  return ;
}

/**
 *  Set the deletion_time member.
 */
void Comment::SetDeletionTime(time_t dt) throw ()
{
  this->timets_[DELETION_TIME] = dt;
  return ;
}

/**
 *  Set the entry_time member.
 */
void Comment::SetEntryTime(time_t et) throw ()
{
  this->timets_[ENTRY_TIME] = et;
  return ;
}

/**
 *  Set the entry_type member.
 */
void Comment::SetEntryType(short et) throw ()
{
  this->shorts_[ENTRY_TYPE] = et;
  return ;
}

/**
 *  Set the expiration_time member.
 */
void Comment::SetExpirationTime(time_t et) throw ()
{
  this->timets_[EXPIRATION_TIME] = et;
  return ;
}

/**
 *  Set the expires member.
 */
void Comment::SetExpires(short e) throw ()
{
  this->shorts_[EXPIRES] = e;
  return ;
}

/**
 *  Set the host member.
 */
void Comment::SetHost(const std::string& h)
{
  this->strings_[HOST] = h;
  return ;
}

/**
 *  Set the is_persistent member.
 */
void Comment::SetIsPersistent(short ip) throw ()
{
  this->shorts_[IS_PERSISTENT] = ip;
  return ;
}

/**
 *  Set the service member.
 */
void Comment::SetService(const std::string& s)
{
  this->strings_[SERVICE] = s;
  return ;
}
