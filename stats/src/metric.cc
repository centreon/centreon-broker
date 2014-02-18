/*
** Copyright 2013 Merethis
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

#include "com/centreon/broker/stats/metric.hh"

using namespace com::centreon::broker::stats;

/**
 *  Default constructor.
 */
metric::metric() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
metric::metric(metric const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
metric::~metric() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
metric& metric::operator=(metric const& right) {
  if (this != &right) {
    _host_id = right._host_id;
    _name = right._name;
    _service_id = right._service_id;
  }
  return (*this);
}

/**
 *  Get the host id.
 *
 *  @return The host id.
 */
unsigned int metric::get_host_id() const throw () {
  return (_host_id);
}

/**
 *  Get the service name.
 *
 *  @return The service name.
 */
std::string const& metric::get_name() const throw () {
  return (_name);
}

/**
 *  Get the service id.
 *
 *  @return The service id.
 */
unsigned int metric::get_service_id() const throw () {
  return (_service_id);
}

/**
 *  Set the host id.
 *
 *  @param[in] host_id The host id.
 */
void metric::set_host_id(unsigned int host_id) throw () {
  _host_id = host_id;
}

/**
 *  Set the service name.
 *
 *  @param[in] name The service name.
 */
void metric::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Set the service id.
 *
 *  @param[in] service_id The service id.
 */
void metric::set_service_id(unsigned int service_id) throw () {
  _service_id = service_id;
}
