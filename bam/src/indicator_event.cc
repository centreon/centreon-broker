/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/indicator_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
indicator_event::indicator_event()
  : status(0),
    in_downtime(false),
    start_time(0),
    duration(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
indicator_event::indicator_event(indicator_event const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
indicator_event::~indicator_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
indicator_event& indicator_event::operator=(indicator_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void indicator_event::_internal_copy(indicator_event const& other) {
  status = other.status;
  in_downtime = other.in_downtime;
  start_time = other.start_time;
  duration = other.duration;
  return ;
}
