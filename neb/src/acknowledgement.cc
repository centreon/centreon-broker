/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/neb/acknowledgement.hh"

using namespace com::centreon::broker::neb;

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
  instance_id = ack.instance_id;
  is_sticky = ack.is_sticky;
  notify_contacts = ack.notify_contacts;
  persistent_comment = ack.persistent_comment;
  service_id = ack.service_id;
  state = ack.state;
  return ;
}

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
    instance_id(0),
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
 *  @return The string "com::centreon::broker::neb::acknowledgement".
 */
QString const& acknowledgement::type() const {
  static QString const ack_type("com::centreon::broker::neb::acknowledgement");
  return (ack_type);
}
