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

#include "com/centreon/broker/bam/metric_listener.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
metric_listener::metric_listener() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
metric_listener::metric_listener(metric_listener const& other) {
  (void)other;
}

/**
 *  Destructor.
 */
metric_listener::~metric_listener() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
metric_listener& metric_listener::operator=(
                                    metric_listener const& other) {
  (void)other;
  return (*this);
}
