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

#include "interface/source_destination.hh"

using namespace interface;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
source_destination::source_destination() {}

/**
 *  @brief Copy constructor.
 *
 *  As source_destination does not hold any data member, this copy
 *  constructor does nothing much than the default constructor.
 *
 *  @param[in] sd Unused.
 *
 *  @see source_destination::source_destination()
 */
source_destination::source_destination(source_destination const& sd)
  : source(sd), destination(sd) {}

/**
 *  @brief Assignment operator.
 *
 *  As source_destination does not hold any data member, this copy
 *  constructor does nothing.
 *
 *  @param[in] sd Unused.
 *
 *  @return This object.
 */
source_destination& source_destination::operator=(source_destination const& sd) {
  (void)sd;
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
source_destination::~source_destination() {}
