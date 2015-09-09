/*
** Copyright 2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
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
  : enable(true),
    poller_id(0),
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
 *  Equality operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return True if both objects are equal.
 */
bool kpi::operator==(kpi const& other) const {
  return ((enable == other.enable)
          && (poller_id == other.poller_id)
          && (kpi_id == other.kpi_id)
          && (kpi_type == other.kpi_type)
          && (ba_id == other.ba_id)
          && (host_id == other.host_id)
          && (service_id == other.service_id)
          && (ba_indicator_id == other.ba_indicator_id)
          && (meta_id == other.meta_id)
          && (boolean_id == other.boolean_id)
          && (drop_warning == other.drop_warning)
          && (drop_critical == other.drop_critical)
          && (drop_unknown == other.drop_unknown));
}

/**
 *  Inequality operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return True if both objects are not equal.
 */
bool kpi::operator!=(kpi const& other) const {
  return (!operator==(other));
}

/**
 *  Get event type.
 *
 *  @return This event type.
 */
unsigned int kpi::type() const {
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
  enable = other.enable;
  poller_id = other.poller_id;
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

