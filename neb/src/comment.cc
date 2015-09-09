/*
** Copyright 2009-2013 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/neb/comment.hh"
#include "com/centreon/broker/neb/internal.hh"

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
    internal_id = c.internal_id;
  persistent = c.persistent;
  service_id = c.service_id;
  source = c.source;
  return ;
}
