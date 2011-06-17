/*
** Copyright 2009-2011 Merethis
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include "com/centreon/broker/neb/comment.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the given object to the current
 *         instance.
 *
 *  This internal method is used to copy data defined inside the comment
 *  class from an object to the current instance. This means that no
 *  superclass data are copied. This method is used in comment copy
 *  constructor and in the assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @see comment(comment const&)
 *  @see operator=(comment const&)
 */
void comment::_internal_copy(comment const& c) {
  author = c.author;
  comment_type = c.comment_type;
  data = c.data;
  deletion_time = c.deletion_time;
  entry_time = c.entry_time;
  entry_type = c.entry_type;
  expire_time = c.expire_time;
  expires = c.expires;
  host_id = c.host_id;
  instance_id = c.instance_id;
  internal_id = c.internal_id;
  persistent = c.persistent;
  service_id = c.service_id;
  source = c.source;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
comment::comment()
  :  comment_type(0),
     deletion_time(0),
     entry_time(0),
     entry_type(0),
     expire_time(0),
     expires(false),
     host_id(0),
     instance_id(0),
     internal_id(0),
     persistent(false),
     service_id(0),
     source(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy data from the given comment to the current instance.
 *
 *  @param[in] c Object to copy.
 */
comment::comment(comment const& c) : io::data(c) {
  _internal_copy(c);
}

/**
 *  Destructor.
 */
comment::~comment() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy data from the given comment to the current instance.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
comment& comment::operator=(comment const& c) {
  io::data::operator=(c);
  _internal_copy(c);
  return (*this);
}

/**
 *  Returns the type of this event.
 *
 *  @return The string "com::centreon::broker::neb::comment".
 */
QString const& comment::type() const {
  static QString const comment_type("com::centreon::broker::neb::comment");
  return (comment_type);
}
