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
#include "interface/ndo/source_destination.hh"

using namespace interface::ndo;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  As source_destination is not copyable, any attempt to use the copy
 *  constructor will result in a call to abort().
 *
 *  @param[in] sd Unused.
 */
source_destination::source_destination(source_destination const& sd)
  : interface::source(),
    interface::destination(),
    base(NULL),
    interface::source_destination(),
    source(NULL),
    destination(NULL) {
  (void)sd;
  assert(false);
  abort();
}

/**
 *  @brief Assignment operator.
 *
 *  As source_destination is not copyable, any attempt to use the
 *  assignment operator will result in a call to abort().
 *
 *  @param[in] sd Unused.
 *
 *  @return This object.
 */
source_destination& source_destination::operator=(source_destination const& sd) {
  (void)sd;
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
 *  @param[in] s Stream on which I/O operations will occur.
 */
source_destination::source_destination(io::stream* s)
  : base(s), source(s), destination(s) {}

/**
 *  Destructor.
 */
source_destination::~source_destination() {}

/**
 *  Close the underlying streams.
 */
void source_destination::close() {
  source::close();
  destination::close();
  return ;
}
