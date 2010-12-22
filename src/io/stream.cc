/*
** Copyright 2009-2010 MERETHIS
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

#include "io/stream.hh"

using namespace io;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stream::stream() {}

/**
 *  @brief Copy constructor.
 *
 *  As stream does not hold any data member, this constructor does
 *  nothing.
 *
 *  @param[in] s Unused.
 */
stream::stream(stream const& s) {
  (void)s;
}

/**
 *  @brief Assignment operator.
 *
 *  As stream does not hold any data member, this method does nothing.
 *
 *  @param[in] s Unused.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
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
stream::~stream() {}
