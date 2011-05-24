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
*/

#include "interface/destination.hh"

using namespace com::centreon::broker::interface;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
destination::destination() {}

/**
 *  @brief Copy constructor.
 *
 *  As destination is an interface object with no data members, the copy
 *  constructor does nothing.
 *
 *  @param[in] dest Unused.
 */
destination::destination(destination const& dest) {
  (void)dest;
}

/**
 *  @brief Assignment operator.
 *
 *  As destination is an interface object with no data members, the
 *  assignment operator does nothing.
 *
 *  @param[in] dest Unused.
 *
 *  @return This object.
 */
destination& destination::operator=(destination const& dest) {
  (void)dest;
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
destination::~destination() {}
