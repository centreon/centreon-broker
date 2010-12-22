/*
** Copyright 2009-2010 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include "events/program_status.hh"

using namespace events;

/**************************************
*                                     *
*          Private Methods            *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the program_status object to the
 *         current instance.
 *
 *  Copy data defined within the program_status class. This method is
 *  used by the copy constructor and the assignment operator.
 *
 *  @param[in] ps Object to copy.
 */
void program_status::_internal_copy(program_status const& ps) {
  active_host_checks_enabled = ps.active_host_checks_enabled;
  active_service_checks_enabled = ps.active_service_checks_enabled;
  check_hosts_freshness = ps.check_hosts_freshness;
  check_services_freshness = ps.check_services_freshness;
  global_host_event_handler = ps.global_host_event_handler;
  global_service_event_handler = ps.global_service_event_handler;
  instance_address = ps.instance_address;
  instance_description = ps.instance_description;
  instance_id = ps.instance_id;
  last_alive = ps.last_alive;
  last_command_check = ps.last_command_check;
  last_log_rotation = ps.last_log_rotation;
  modified_host_attributes = ps.modified_host_attributes;
  modified_service_attributes = ps.modified_service_attributes;
  obsess_over_hosts = ps.obsess_over_hosts;
  obsess_over_services = ps.obsess_over_services;
  passive_host_checks_enabled = ps.passive_host_checks_enabled;
  passive_service_checks_enabled = ps.passive_service_checks_enabled;
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
 *  Initialize members to 0, NULL or equivalent.
 */
program_status::program_status()
  : active_host_checks_enabled(false),
    active_service_checks_enabled(false),
    check_hosts_freshness(false),
    check_services_freshness(false),
    instance_id(0),
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
 *  @param[in] ps Object to copy data from.
 */
program_status::program_status(program_status const& ps) : status(ps) {
  _internal_copy(ps);
}

/**
 *  Destructor.
 */
program_status::~program_status() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy all members of the given object to the current instance.
 *
 *  @param[in] ps Object to copy.
 */
program_status& program_status::operator=(program_status const& ps) {
  status::operator=(ps);
  _internal_copy(ps);
  return (*this);
}

/**
 *  @brief Get the type of the event (event::PROGRAMSTATUS).
 *
 *  This method is used to determine the type of the event at runtime.
 *
 *  @return event::PROGRAMSTATUS
 */
int program_status::get_type() const {
  return (PROGRAMSTATUS);
}
