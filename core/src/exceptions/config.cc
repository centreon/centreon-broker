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

#include "com/centreon/broker/exceptions/config.hh"

using namespace com::centreon::broker::exceptions;

/**
 *  Default constructor.
 */
config::config() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
config::config(config const& other) : msg(other) {}

/**
 *  Destructor.
 */
config::~config() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
config& config::operator=(config const& other) {
  msg::operator=(other);
  return (*this);
}
