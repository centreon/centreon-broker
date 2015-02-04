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

#include "com/centreon/broker/bam/dimension_kpi_event.hh"
#include "com/centreon/broker/bam/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
dimension_kpi_event::dimension_kpi_event()
  : kpi_id(0),
    ba_id(0),
    host_id(0),
    service_id(0),
    kpi_ba_id(0),
    meta_service_id(0),
    boolean_id(0),
    impact_warning(0),
    impact_critical(0),
    impact_unknown(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
dimension_kpi_event::dimension_kpi_event(dimension_kpi_event const& other)
  : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
dimension_kpi_event::~dimension_kpi_event() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
dimension_kpi_event& dimension_kpi_event::operator=(
    dimension_kpi_event const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Equality test operator.
 *
 *  @param[in] other  The object to test for equality.
 *
 *  @return  True if the two objects are equal.
 */
bool dimension_kpi_event::operator==(
    dimension_kpi_event const& other) const {
  return ((kpi_id == other.kpi_id)
          && (ba_id == other.ba_id)
          && (ba_name == other.ba_name)
          && (host_id == other.host_id)
          && (host_name == other.host_name)
          && (service_id == other.service_id)
          && (service_description == other.service_description)
          && (kpi_ba_id == other.kpi_ba_id)
          && (kpi_ba_name == other.kpi_ba_name)
          && (meta_service_id == other.meta_service_id)
          && (meta_service_name == other.meta_service_name)
          && (boolean_id == other.boolean_id)
          && (boolean_name == other.boolean_name)
          && (impact_warning == other.impact_warning)
          && (impact_critical == other.impact_critical)
          && (impact_unknown == other.impact_unknown));
}


/**
 *  Get the event type.
 *
 *  @return Event type.
 */
unsigned int dimension_kpi_event::type() const {
  return (io::events::data_type<io::events::bam, bam::de_dimension_kpi_event>::value);
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other Object to copy.
 */
void dimension_kpi_event::_internal_copy(dimension_kpi_event const& other) {
  kpi_id = other.kpi_id;
  ba_id = other.ba_id;
  ba_name = other.ba_name;
  host_id = other.host_id;
  host_name = other.host_name;
  service_id = other.service_id;
  service_description = other.service_description;
  kpi_ba_id = other.kpi_ba_id;
  kpi_ba_name = other.kpi_ba_name;
  meta_service_id = other.meta_service_id;
  meta_service_name = other.meta_service_name;
  boolean_id = other.boolean_id;
  boolean_name = other.boolean_name;
  impact_warning = other.impact_warning;
  impact_critical = other.impact_critical;
  impact_unknown = other.impact_unknown;
  return ;
}
