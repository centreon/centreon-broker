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

#include "serialization/serial.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::serialization;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
serial::serial() {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
serial::serial(serial const& s)
  : io::istream(s), iserial(s), io::ostream(s), oserial(s) {}

/**
 *  Destructor.
 */
serial::~serial() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
serial& serial::operator=(serial const& s) {
  iserial::operator=(s);
  oserial::operator=(s);
  return (*this);
}
