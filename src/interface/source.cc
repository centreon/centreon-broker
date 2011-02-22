/*
** Copyright 2009-2011 MERETHIS
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
**
** For more information: contact@centreon.com
*/

#include <limits.h>
#include "interface/source.hh"

using namespace interface;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
source::source() {}

/**
 *  @brief Copy constructor.
 *
 *  As source is an interface object with no data members, the copy
 *  constructor does nothing.
 *
 *  @param[in] s Unused.
 */
source::source(source const& s) {
  (void)s;
}

/**
 *  @brief Assignment operator.
 *
 *  As source is an interface object with no data members, the
 *  assignment operator does nothing.
 *
 *  @param[in] s Unused.
 *
 *  @return This object.
 */
source& source::operator=(source const& s) {
  (void)s;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destructor.
 */
source::~source() {}

/**
 *  Get the number of remaining events.
 *
 *  @return Number of events available from the source.
 */
unsigned int source::size() const {
  return (UINT_MAX);
}
