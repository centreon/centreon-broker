/*
** Copyright 2009-2012 Merethis
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
#include "com/centreon/broker/storage/metric.hh"
#include "com/centreon/broker/storage/perfdata.hh"

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
  : ctime(0),
    interval(0),
    metric_id(0),
    rrd_len(0),
    value(NAN),
    value_type(perfdata::gauge) {}

/**
 *  Copy constructor.
 *
 *  @param[in] m Object to copy.
 */
metric::metric(metric const& m) : io::data(m) {
  _internal_copy(m);
}

/**
 *  Destructor.
 */
metric::~metric() {}

/**
 *  Assignment operator.
 *
 *  @param[in] m Object to copy.
 *
 *  @return This object.
 */
metric& metric::operator=(metric const& m) {
  io::data::operator=(m);
  _internal_copy(m);
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The string "com::centreon::broker::storage::metric".
 */
QString const& metric::type() const {
  static QString const metric_type("com::centreon::broker::storage::metric");
  return (metric_type);
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
void metric::_internal_copy(metric const& m) {
  ctime = m.ctime;
  interval = m.interval;
  metric_id = m.metric_id;
  name = m.name;
  rrd_len = m.rrd_len;
  value = m.value;
  value_type = m.value_type;
  return ;
}
