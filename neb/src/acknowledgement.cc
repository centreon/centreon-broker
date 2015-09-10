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
#include "com/centreon/broker/neb/acknowledgement.hh"
#include "com/centreon/broker/neb/internal.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief acknowledgement default constructor.
 *
 *  acknowledgement default constructor. Set all members to their
 *  default value (0, NULL or equivalent).
 */
acknowledgement::acknowledgement()
  : acknowledgement_type(0),
    deletion_time(0),
    entry_time(0),
    host_id(0),
        is_sticky(false),
    notify_contacts(false),
    persistent_comment(false),
    service_id(0),
    state(0) {}

/**
 *  @brief acknowledgement copy constructor.
 *
 *  Copy data from the acknowledgement object to the current instance.
 *
 *  @param[in] ack Object to copy.
 */
acknowledgement::acknowledgement(acknowledgement const& ack)
  : io::data(ack) {
  _internal_copy(ack);
}

/**
 *  Destructor.
 */
acknowledgement::~acknowledgement() {}

/**
 *  Assignment operator.
 *
 *  @param[in] ack Object to copy.
 *
 *  @return This object.
 */
acknowledgement& acknowledgement::operator=(acknowledgement const& ack) {
  io::data::operator=(ack);
  _internal_copy(ack);
  return (*this);
}

/**
 *  @brief Get the type of the event.
 *
 *  Return the type of this event. This can be useful for runtime data
 *  type determination.
 *
 *  @return The event type.
 */
unsigned int acknowledgement::type() const {
  return (io::events::data_type<io::events::neb, neb::de_acknowledgement>::value);
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the given object to the current
 *         instance.
 *
 *  This internal method is used to copy data defined inside the
 *  acknowledgement class from an object to the current instance. This
 *  means that no superclass data are copied. This method is used in
 *  acknowledgement copy constructor and in the assignment operator.
 *
 *  @param[in] ack Object to copy.
 *
 *  @see acknowledgement(acknowledgement const&)
 *  @see operator=(acknowledgement const&)
 */
void acknowledgement::_internal_copy(acknowledgement const& ack) {
  acknowledgement_type = ack.acknowledgement_type;
  author = ack.author;
  comment = ack.comment;
  deletion_time = ack.deletion_time;
  entry_time = ack.entry_time;
  host_id = ack.host_id;
    is_sticky = ack.is_sticky;
  notify_contacts = ack.notify_contacts;
  persistent_comment = ack.persistent_comment;
  service_id = ack.service_id;
  state = ack.state;
  return ;
}
