/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/neb/instance_status.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Initialize members to 0, NULL or equivalent.
 */
instance_status::instance_status()
  : active_host_checks_enabled(false),
    active_service_checks_enabled(false),
    check_hosts_freshness(false),
    check_services_freshness(false),
    daemon_mode(false),
    id(0),
    last_alive(0),
    last_command_check(0),
    last_log_rotation(0),
    modified_host_attributes(0),
    modified_service_attributes(0),
    obsess_over_hosts(false),
    obsess_over_services(false),
    passive_host_checks_enabled(false),
    passive_service_checks_enabled(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] is Object to copy data from.
 */
instance_status::instance_status(instance_status const& is) : status(is) {
  _internal_copy(is);
}

/**
 *  Destructor.
 */
instance_status::~instance_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] is Object to copy.
 */
instance_status& instance_status::operator=(instance_status const& is) {
  status::operator=(is);
  _internal_copy(is);
  return (*this);
}

/**
 *  Get the type of the event.
 *
 *  @return The string "com::centreon::broker::neb::instance_status".
 */
unsigned int instance_status::type() const {
  return (io::data::data_type(io::data::neb, neb::instance_status));
}

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the instance_status object to the
 *         current instance.
 *
 *  Copy data defined within the instance_status class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  @param[in] is Object to copy.
 */
void instance_status::_internal_copy(instance_status const& is) {
  active_host_checks_enabled = is.active_host_checks_enabled;
  active_service_checks_enabled = is.active_service_checks_enabled;
  address = is.address;
  check_hosts_freshness = is.check_hosts_freshness;
  check_services_freshness = is.check_services_freshness;
  daemon_mode = is.daemon_mode;
  description = is.description;
  global_host_event_handler = is.global_host_event_handler;
  global_service_event_handler = is.global_service_event_handler;
  id = is.id;
  last_alive = is.last_alive;
  last_command_check = is.last_command_check;
  last_log_rotation = is.last_log_rotation;
  modified_host_attributes = is.modified_host_attributes;
  modified_service_attributes = is.modified_service_attributes;
  obsess_over_hosts = is.obsess_over_hosts;
  obsess_over_services = is.obsess_over_services;
  passive_host_checks_enabled = is.passive_host_checks_enabled;
  passive_service_checks_enabled = is.passive_service_checks_enabled;
  return ;
}
