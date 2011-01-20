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

#include "io/acceptor.hh"

using namespace io;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
acceptor::acceptor() {}

/**
 *  @brief Copy constructor.
 *
 *  As acceptor does not hold any data member, this constructor does nothing.
 *
 *  @param[in] a Unused.
 */
acceptor::acceptor(acceptor const& a) {
  (void)a;
}

/**
 *  @brief Assignment operator.
 *
 *  As acceptor does not hold any data member, this method does nothing.
 *
 *  @param[in] a Unused.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  (void)a;
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
acceptor::~acceptor() {}
