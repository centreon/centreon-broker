/*
** Copyright 2009-2013 Merethis
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

#include "com/centreon/broker/neb/host.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize internal data to 0, NULL or equivalent.
 */
host::host() {
  _zero_initialize();
}

/**
 *  @brief Build a host from a host_status.
 *
 *  Copy host_status data to the current instance and zero-initialize
 *  other members.
 *
 *  @param[in] hs host_status object to initialize part of the host
 *                instance.
 */
host::host(host_status const& hs) : host_status(hs) {
  _zero_initialize();
}

/**
 *  @brief Copy constructor.
 *
 *  Copy data from the given object to the current instance.
 *
 *  @param[in] h Object to copy.
 */
host::host(host const& h) : host_service(h), host_status(h) {
  _internal_copy(h);
}

/**
 *  Destructor.
 */
host::~host() {}

/**
 *  @brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  @param[in] h Object to copy.
 *
 *  @return This object.
 */
host& host::operator=(host const& h) {
  host_service::operator=(h);
  host_status::operator=(h);
  _internal_copy(h);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The string "com::centreon::broker::neb::host".
 */
unsigned int host::type() const {
  return (io::data::data_type(io::data::neb, neb::host));
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy all internal data of the given object to the current
 *         instance.
 *
 *  This method copy all data defined directly in the host class. This
 *  is used by the copy constructor and the assignment operator.
 *
 *  @param[in] h Object to copy data.
 */
void host::_internal_copy(host const& h) {
  address = h.address;
  alias = h.alias;
  flap_detection_on_down = h.flap_detection_on_down;
  flap_detection_on_unreachable = h.flap_detection_on_unreachable;
  flap_detection_on_up = h.flap_detection_on_up;
  host_name = h.host_name;
  instance_id = h.instance_id;
  notify_on_down = h.notify_on_down;
  notify_on_unreachable = h.notify_on_unreachable;
  stalk_on_down = h.stalk_on_down;
  stalk_on_unreachable = h.stalk_on_unreachable;
  stalk_on_up = h.stalk_on_up;
  statusmap_image = h.statusmap_image;
  return ;
}

/**
 *  @brief Zero-initialize internal data.
 *
 *  This method is used by constructors.
 */
void host::_zero_initialize() {
  flap_detection_on_down = 0;
  flap_detection_on_unreachable = 0;
  flap_detection_on_up = 0;
  instance_id = 0;
  notify_on_down = 0;
  notify_on_unreachable = 0;
  stalk_on_down = 0;
  stalk_on_unreachable = 0;
  stalk_on_up = 0;
  return ;
}
