/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/internal.hh"
#include "com/centreon/broker/io/events.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
kpi::kpi()
  : activate(true),
    kpi_id(0),
    kpi_type(0),
    ba_id(0),
    host_id(0),
    service_id(0),
    ba_indicator_id(0),
    meta_id(0),
    boolean_id(0),
    drop_warning(0.0),
    drop_critical(0.0),
    drop_unknown(0.0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
kpi::kpi(kpi const& other) : io::data(other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
kpi::~kpi() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
kpi& kpi::operator=(kpi const& other) {
  if (this != &other) {
    io::data::operator=(other);
    _internal_copy(other);
  }
  return (*this);
}

/**
 *  Get event type.
 *
 *  @return This event type.
 */
unsigned int kpi::type() const {
  return (static_type());
}

/**
 *  Get event class type.
 *
 *  @return Event class type.
 */
unsigned int kpi::static_type() {
  return (io::events::data_type<io::events::dumper, dumper::de_entries_kpi>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void kpi::_internal_copy(kpi const& other) {
  activate = other.activate;
  kpi_id = other.kpi_id;
  kpi_type = other.kpi_type;
  ba_id = other.ba_id;
  host_id = other.host_id;
  service_id = other.service_id;
  ba_indicator_id = other.ba_indicator_id;
  meta_id = other.meta_id;
  boolean_id = other.boolean_id;
  drop_warning = other.drop_warning;
  drop_critical = other.drop_critical;
  drop_unknown = other.drop_unknown;
  return ;
}
