/*
** Copyright 2014-2015 Merethis
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

#include "com/centreon/broker/bam/service_listener.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
service_listener::service_listener() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
service_listener::service_listener(service_listener const& other) {
  (void)other;
}

/**
 *  Destructor.
 */
service_listener::~service_listener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
service_listener& service_listener::operator=(
                                      service_listener const& other) {
  (void)other;
  return (*this);
}

/**
 *  Notify of a service status update.
 *
 *  @param[in]  status   Service status.
 *  @param[out] visitor  Visitor.
 */
void service_listener::service_update(
                         misc::shared_ptr<neb::service_status> const& status,
                         io::stream* visitor) {
  (void)status;
  (void)visitor;
  return ;
}

/**
 *  Notify of an acknowledgement.
 *
 *  @param[in]  ack      Acknowledgement.
 *  @param[out] visitor  Visitor.
 */
void service_listener::service_update(
                         misc::shared_ptr<notification::acknowledgement> const& ack,
                         io::stream* visitor) {
  (void)ack;
  (void)visitor;
  return ;
}

/**
 *  Notify of a downtime.
 *
 *  @param[in]  dt       Downtime.
 *  @param[out] visitor  Visitor.
 */
void service_listener::service_update(
                         misc::shared_ptr<notification::downtime> const& dt,
                         io::stream* visitor) {
  (void)dt;
  (void)visitor;
  return ;
}
