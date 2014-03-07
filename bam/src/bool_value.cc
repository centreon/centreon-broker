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

#include "com/centreon/broker/bam/bool_value.hh"

using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
bool_value::bool_value() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
bool_value::bool_value(bool_value const& right) : computable(right) {}

/**
 *  Destructor.
 */
bool_value::~bool_value() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
bool_value& bool_value::operator=(bool_value const& right) {
  computable::operator=(right);
  return (*this);
}
