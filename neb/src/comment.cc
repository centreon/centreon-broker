/*
** Copyright 2009-2013,2015 Merethis
**
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

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/comment.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

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
     internal_id(0),
     persistent(false),
     service_id(0),
     source(0) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy data from the given comment to the current instance.
 *
 *  @param[in] other  Object to copy.
 */
comment::comment(comment const& other) : io::data(other) {
  _internal_copy(other);
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
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
comment& comment::operator=(comment const& other) {
  io::data::operator=(other);
  _internal_copy(other);
  return (*this);
}

/**
 *  Returns the type of this event.
 *
 *  @return The event type.
 */
unsigned int comment::type() const {
  return (io::events::data_type<io::events::neb, neb::de_comment>::value);
}

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
 *  @param[in] other  Object to copy.
 *
 *  @see comment(comment const&)
 *  @see operator=(comment const&)
 */
void comment::_internal_copy(comment const& other) {
  author = other.author;
  comment_type = other.comment_type;
  data = other.data;
  deletion_time = other.deletion_time;
  entry_time = other.entry_time;
  entry_type = other.entry_type;
  expire_time = other.expire_time;
  expires = other.expires;
  host_id = other.host_id;
  internal_id = other.internal_id;
  persistent = other.persistent;
  service_id = other.service_id;
  source = other.source;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const comment::entries[] = {
  mapping::entry(
    &comment::author,
    "author",
    1),
  mapping::entry(
    &comment::comment_type,
    "type",
    2),
  mapping::entry(
    &comment::deletion_time,
    "deletion_time",
    3,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &comment::entry_time,
    "entry_time",
    4),
  mapping::entry(
    &comment::entry_type,
    "entry_type",
    5),
  mapping::entry(
    &comment::expire_time,
    "expire_time",
    6),
  mapping::entry(
    &comment::expires,
    "expires",
    7),
  mapping::entry(
    &comment::host_id,
    "host_id",
    8,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &comment::instance_id,
    "instance_id",
    9,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &comment::internal_id,
    "internal_id",
    10),
  mapping::entry(
    &comment::persistent,
    "persistent",
    11),
  mapping::entry(
    &comment::service_id,
    "service_id",
    12,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &comment::source,
    "source",
    13),
  mapping::entry(
    &comment::data,
    "data",
    14),
  mapping::entry()
};

// Operations.
static io::data* new_comment() {
  return (new comment);
}
io::event_info::event_operations const comment::operations = {
  &new_comment
};
