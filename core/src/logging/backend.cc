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

#include "logging/backend.hh"

using namespace com::centreon::broker::logging;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
backend::backend() {}

/**
 *  Copy constructor.
 *
 *  @param[in] b Unused.
 */
backend::backend(backend const& b) {
  (void)b;
}

/**
 *  Destructor.
 */
backend::~backend() {}

/**
 *  Assignment operator.
 *
 *  @param[in] b Unused.
 *
 *  @return This object..
 */
backend& backend::operator=(backend const& b) {
  (void)b;
  return (*this);
}
