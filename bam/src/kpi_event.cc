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

#include "com/centreon/broker/bam/kpi_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
kpi_event::kpi_event()
  : kpi_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other Object to copy.
 */
kpi_event::kpi_event(kpi_event const& other) : indicator_event(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
kpi_event::~kpi_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
kpi_event& kpi_event::operator=(kpi_event const& other) {
  if (this != &other) {
    indicator_event::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int kpi_event::type() const {
  return (io::events::data_type<io::events::bam, bam::de_kpi_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void kpi_event::_internal_copy(kpi_event const& other) {
  kpi_id = other.kpi_id;
  return ;
}
