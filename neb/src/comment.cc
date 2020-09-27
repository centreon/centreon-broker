/*
** Copyright 2009-2013,2015,2019-2020 Centreon
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

#include "com/centreon/broker/neb/comment.hh"

#include "com/centreon/broker/database/table_max_size.hh"

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
    : io::data(comment::static_type()),
      comment_type(0),
      deletion_time(0),
      entry_time(0),
      entry_type(0),
      expire_time(0),
      expires(false),
      host_id(0),
      internal_id(0),
      persistent(false),
      poller_id(0),
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
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return *this;
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
  poller_id = other.poller_id;
  service_id = other.service_id;
  source = other.source;
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const comment::entries[] = {
    mapping::entry(&comment::author,
                   "author",
                   get_comments_col_size(comments_author)),
    mapping::entry(&comment::comment_type, "type"),
    mapping::entry(&comment::data,
                   "data",
                   get_comments_col_size(comments_data)),
    mapping::entry(&comment::deletion_time,
                   "deletion_time",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&comment::entry_time,
                   "entry_time",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&comment::entry_type, "entry_type"),
    mapping::entry(&comment::expire_time,
                   "expire_time",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&comment::expires, "expires"),
    mapping::entry(&comment::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&comment::internal_id, "internal_id"),
    mapping::entry(&comment::persistent, "persistent"),
    mapping::entry(&comment::poller_id,
                   "instance_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&comment::service_id,
                   "service_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&comment::source, "source"),
    mapping::entry()};

// Operations.
static io::data* new_comment() {
  return new comment;
}
io::event_info::event_operations const comment::operations = {&new_comment};
