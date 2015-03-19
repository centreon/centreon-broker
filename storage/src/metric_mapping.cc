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

#include <cmath>
#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/metric_mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
metric_mapping::metric_mapping()
  : metric_id(0),
    status_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] m Object to copy.
 */
metric_mapping::metric_mapping(metric_mapping const& m) : io::data(m) {
  _internal_copy(m);
}

/**
 *  Destructor.
 */
metric_mapping::~metric_mapping() {}

/**
 *  Assignment operator.
 *
 *  @param[in] m Object to copy.
 *
 *  @return This object.
 */
metric_mapping& metric_mapping::operator=(metric_mapping const& m) {
  io::data::operator=(m);
  _internal_copy(m);
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The event type.
 */
unsigned int metric_mapping::type() const {
  return (metric_mapping::static_type());
}

/**
 *  Get the type of this event.
 *
 *  @return  The event type.
 */
unsigned int metric_mapping::static_type() {
  return (io::events::data_type<io::events::storage, storage::de_metric_mapping>::value);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] m Object to copy.
 */
void metric_mapping::_internal_copy(metric_mapping const& m) {
  metric_id = m.metric_id;
  status_id = m.status_id;
  return ;
}

/**************************************
*                                     *
*           Static Objects            *
*                                     *
**************************************/

// Mapping.
mapping::entry const metric_mapping::entries[] = {
  mapping::entry(
    &metric_mapping::metric_id,
    "metric_id",
    1,
    mapping::entry::NULL_ON_ZERO),
  mapping::entry(
    &metric_mapping::status_id,
    "status_id",
    1,
  mapping::entry::NULL_ON_ZERO),
  mapping::entry()
};

// Operations.
static io::data* new_metric_mapping() {
  return (new metric_mapping);
}
io::event_info::event_operations const metric_mapping::operations = {
  &new_metric_mapping
};
