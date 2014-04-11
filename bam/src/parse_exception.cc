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

#include  "com/centreon/broker/bam/parse_exception.hh"

using namespace com::centreon::broker::bam;

/**
 *  Constructor
 */
parse_exception::parse_exception() : msg() {}

/**
 *  Copy constructor
 *
 *  @param[in] Other object
 */
parse_exception::parse_exception(parse_exception const& other) : msg(other) {}

/**
 *  Destructor
 */
parse_exception::~parse_exception() throw() {}

/**
 *  Assignment operator
 *
 *  @param[in] other Object to copy.
 *
 *  @return This object.
 */
parse_exception& parse_exception::operator=(parse_exception const& other) {
  msg::operator=(other);
  return (*this);
}
