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

#include "events/comment.hh"

using namespace Events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the given object to the current instance.
 *
 *  This internal method is used to copy data defined inside the Comment class
 *  from an object to the current instance. This means that no superclass data
 *  are copied. This method is used in Comment copy constructor and in the =
 *  operator overload.
 *
 *  @param[in] comment Object to copy from.
 *
 *  @see Comment(comment const&)
 *  @see operator=(comment const&)
 */
void comment::_internal_copy(comment const& c) {
  this->author              = c.author;
  this->data                = c.data;
  this->deletion_time       = c.deletion_time;
  this->entry_time          = c.entry_time;
  this->entry_type          = c.entry_type;
  this->expire_time         = c.expire_time;
  this->expires             = c.expires;
  this->host_name           = c.host_name;
  this->instance_name       = c.instance_name;
  this->internal_id         = c.internal_id;
  this->persistent          = c.persistent;
  this->service_description = c.service_description;
  this->source              = c.source;
  this->type                = c.type;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Comment default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
comment::comment()
  :  deletion_time(0),
     entry_time(0),
     entry_type(0),
     expire_time(0),
     expires(false),
     internal_id(0),
     persistent(false),
     source(0),
     type(0) {}

/**
 *  @brief Comment copy constructor.
 *
 *  Copy data from the given comment to the current instance.
 *
 *  @param[in] comment Object to copy from.
 */
comment::comment(comment const& c) : Event(c) {
  _internal_copy(c);
}

/**
 *  Comment destructor.
 */
comment::~comment() {}

/**
 *  @brief Overload of the assignment operator.
 *
 *  Copy data from the given comment to the current instance.
 *
 *  @param[in] comment Object to copy from.
 *
 *  @return *this
 */
comment& comment::operator=(comment const& c) {
  Event::operator=(c);
  _internal_copy(c);
  return (*this);
}

/**
 *  @brief Returns the type of this event (Event::COMMENT).
 *
 *  The type of this event can be useful for runtime determination of the type
 *  of an event.
 *
 *  @see Event
 *
 *  @return Event::COMMENT
 */
int comment::GetType() const {
  return (Event::COMMENT);
}
