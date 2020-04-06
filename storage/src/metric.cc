/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/storage/metric.hh"

#include <cassert>
#include <cmath>

#include "com/centreon/broker/io/events.hh"
#include "com/centreon/broker/storage/internal.hh"
#include "com/centreon/broker/storage/perfdata.hh"

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
metric::metric()
    : io::data(metric::static_type()),
      ctime(0),
      interval(0),
      is_for_rebuild(false),
      metric_id(0),
      rrd_len(0),
      value(NAN),
      value_type(perfdata::gauge),
      host_id(0),
      service_id(0) {}

metric::metric(uint32_t host_id,
               uint32_t service_id,
               std::string const& name,
               timestamp ctime,
               uint32_t interval,
               bool is_for_rebuild,
               uint32_t metric_id,
               int32_t rrd_len,
               double value,
               short value_type)
    : io::data(metric::static_type()),
      ctime{ctime},
      interval{interval},
      is_for_rebuild{is_for_rebuild},
      metric_id{metric_id},
      name{name},
      rrd_len{rrd_len},
      value{value},
      value_type{value_type},
      host_id{host_id},
      service_id{service_id} {
  assert(rrd_len != 0);
}

/**************************************
 *                                     *
 *           Static Objects            *
 *                                     *
 **************************************/

// Mapping.
mapping::entry const metric::entries[] = {
    mapping::entry(&metric::ctime,
                   "ctime",
                   mapping::entry::invalid_on_minus_one),
    mapping::entry(&metric::interval, "interval"),
    mapping::entry(&metric::metric_id,
                   "metric_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&metric::name, "name"),
    mapping::entry(&metric::rrd_len, "rrd_len"),
    mapping::entry(&metric::value, "value"),
    mapping::entry(&metric::value_type, "value_type"),
    mapping::entry(&metric::is_for_rebuild, "is_for_rebuild"),
    mapping::entry(&metric::host_id,
                   "host_id",
                   mapping::entry::invalid_on_zero),
    mapping::entry(&metric::service_id, "service_id"),
    mapping::entry()};

// Operations.
static io::data* new_metric() {
  return new metric;
}
io::event_info::event_operations const metric::operations = {&new_metric};
