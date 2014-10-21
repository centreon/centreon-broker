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

#include "com/centreon/broker/bam/event_parent.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
event_parent::event_parent()
  : kpi_id(0),
    ba_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
event_parent::event_parent(event_parent const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
event_parent::~event_parent() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
event_parent& event_parent::operator=(event_parent const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int event_parent::type() const {
  return (io::events::data_type<io::events::bam, bam::de_event_parent>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void event_parent::_internal_copy(event_parent const& other) {
  kpi_id = other.kpi_id;
  ba_id = other.ba_id;
  ba_start_time = other.ba_start_time;
  ba_end_time = other.ba_end_time;
  return ;
}
