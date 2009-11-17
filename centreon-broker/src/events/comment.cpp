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

#include "events/comment.h"

using namespace CentreonBroker::Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal data of the given object to the current instance.
 *
 *  This internal method is used to copy data defined inside the Comment class
 *  from an object to the current instance. This means that no superclass data
 *  are copied. This method is used in Comment copy constructor and in the =
 *  operator overload.
 *
 *  \param[in] comment Object to copy from.
 *
 *  \see Comment(const Comment&)
 *  \see operator=
 */
void Comment::InternalCopy(const Comment& comment)
{
  this->author        = comment.author;
  this->comment       = comment.comment;
  this->comment_time  = comment.comment_time;
  this->comment_type  = comment.comment_type;
  this->deletion_time = comment.deletion_time;
  this->entry_time    = comment.entry_time;
  this->entry_type    = comment.entry_type;
  this->expire_time   = comment.expire_time;
  this->expires       = comment.expires;
  this->host          = comment.host;
  this->internal_id   = comment.internal_id;
  this->persistent    = comment.persistent;
  this->service       = comment.service;
  this->source        = comment.source;
  this->type          = comment.type;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Comment default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
Comment::Comment()
  :  comment_time(0),
     comment_type(0),
     deletion_time(0),
     entry_time(0),
     entry_type(0),
     expire_time(0),
     expires(false),
     internal_id(0),
     persistent(false),
     source(0),
     type(0) {}

/**
 *  \brief Comment copy constructor.
 *
 *  Copy data from the given comment to the current instance.
 *
 *  \param[in] comment Object to copy from.
 */
Comment::Comment(const Comment& comment) : Event(comment)
{
  this->InternalCopy(comment);
}

/**
 *  Comment destructor.
 */
Comment::~Comment() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given comment to the current instance.
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
 *  \brief Returns the type of this event (CentreonBroker::Event::COMMENT).
 *
 *  The type of this event can be useful for runtime determination of the type
 *  of an event.
 *
 *  \see CentreonBroker::Event
 *
 *  \return CentreonBroker::Event::COMMENT
 */
int Comment::GetType() const throw ()
{
  return (Event::COMMENT);
}
