/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/rrd/metric.hh"
#include "com/centreon/broker/rrd/status.hh"
#include "mapping.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd;

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
    &metric::name,
    5,
    "name"),
  mapped_data<metric>(
    &metric::rrd_len,
    6,
    "rrd_len"),
  mapped_data<metric>(
    &metric::value,
    7,
    "value"),
  mapped_data<metric>()
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
  mapped_data<status>()
};

template <> const mapped_data<rrd::metric>*
  mapped_type<rrd::metric>::members(metric_mapping);
template <> char const*
  mapped_type<rrd::metric>::table("data_bin");

template <> const mapped_data<rrd::status>*
  mapped_type<rrd::status>::members(status_mapping);
template <> char const*
  mapped_type<rrd::status>::table("data_bin2");
