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

#include "serialization/iserial.hh"

using namespace com::centreon::broker::serialization;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
iserial::iserial() {}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
iserial::iserial(iserial const& i) : io::istream(i) {}

/**
 *  Destructor.
 */
iserial::~iserial() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
iserial& iserial::operator=(iserial const& i) {
  io::istream::operator=(i);
  return (*this);
}
