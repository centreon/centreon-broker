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

#include <assert.h>
#include <stdlib.h>
#include "interface/ndo/base.hh"

using namespace interface::ndo;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  base is not copyable. Any attempt to use the copy constructor will
 *  result in a call to abort().
 *
 *  @param[in] b Unused.
 */
base::base(base const& b) : _stream(NULL) {
  (void)b;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  base is not copyable. Any attempt to use the assignment operator
 *  will result in a call to abort().
 *
 *  @param[in] b Unused.
 *
 *  @return This object.
 */
base& base::operator=(base const& b) {
  (void)b;
  assert(false);
  abort();
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 *
 *  @param[in] s Stream on which NDO protocol will act.
 */
base::base(io::stream* s) : _stream(s) {}

/**
 *  Destructor.
 */
base::~base() {}
