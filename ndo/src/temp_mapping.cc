/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/storage/events.hh"
#include "mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::storage;

static mapped_data<metric> const metric_mapping[] = {
  mapped_data<metric>(
    &metric::ctime,
    1,
    "ctime"),
  mapped_data<metric>(
    &metric::interval,
    2,
    "interval"),
  mapped_data<metric>(
    &metric::metric_id,
    3,
    "metric_id"),
  mapped_data<metric>(
    &metric::name,
    4,
    "name"),
  mapped_data<metric>(
    &metric::rrd_len,
    6,
    "rrd_len"),
  mapped_data<metric>(
    &metric::value,
    7,
    "value"),
  mapped_data<metric>(
    &metric::value_type,
    8,
    "value_type"),
  mapped_data<metric>(
    &metric::is_for_rebuild,
    9,
    "is_for_rebuild"),
  mapped_data<metric>()
};

static mapped_data<rebuild> const rebuild_mapping[] = {
  mapped_data<rebuild>(
    &rebuild::end,
    1,
    "end"),
  mapped_data<rebuild>(
    &rebuild::id,
    2,
    "id"),
  mapped_data<rebuild>(
    &rebuild::is_index,
    3,
    "is_index"),
  mapped_data<rebuild>()
};

static mapped_data<remove_graph> const remove_graph_mapping[] = {
  mapped_data<remove_graph>(
    &remove_graph::id,
    1,
    "id"),
  mapped_data<remove_graph>(
    &remove_graph::is_index,
    2,
    "is_end"),
  mapped_data<remove_graph>()
};

static mapped_data<status> const status_mapping[] = {
  mapped_data<status>(
    &status::ctime,
    1,
    "ctime"),
  mapped_data<status>(
    &status::index_id,
    2,
    "index_id"),
  mapped_data<status>(
    &status::interval,
    3,
    "interval"),
  mapped_data<status>(
    &status::rrd_len,
    4,
    "rrd_len"),
  mapped_data<status>(
    &status::state,
    5,
    "state"),
  mapped_data<status>(
    &status::is_for_rebuild,
    6,
    "is_for_rebuild"),
  mapped_data<status>()
};

namespace     com {
  namespace   centreon {
    namespace broker {
      template <> const mapped_data<storage::metric>*
        mapped_type<storage::metric>::members(metric_mapping);
      template <> char const*
        mapped_type<storage::metric>::table("data_bin");

      template <> const mapped_data<storage::rebuild>*
        mapped_type<storage::rebuild>::members(rebuild_mapping);
      template <> char const*
        mapped_type<storage::rebuild>::table("metrics");

      template <> const mapped_data<storage::remove_graph>*
        mapped_type<storage::remove_graph>::members(remove_graph_mapping);
      template <> char const*
        mapped_type<storage::remove_graph>::table("index_data");

      template <> const mapped_data<storage::status>*
        mapped_type<storage::status>::members(status_mapping);
      template <> char const*
        mapped_type<storage::status>::table("data_bin2");
    }
  }
}
