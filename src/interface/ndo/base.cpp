/*
**  Copyright 2009 MERETHIS
**  This file is part of CentreonBroker.
**
**  CentreonBroker is free software: you can redistribute it and/or modify it
**  under the terms of the GNU General Public License as published by the Free
**  Software Foundation, either version 2 of the License, or (at your option)
**  any later version.
**
**  CentreonBroker is distributed in the hope that it will be useful, but
**  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
**  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
**  for more details.
**
**  You should have received a copy of the GNU General Public License along
**  with CentreonBroker.  If not, see <http://www.gnu.org/licenses/>.
**
**  For more information : contact@centreon.com
*/

#include <assert.h>
#include <stdlib.h>             // for abort
#include "interface/ndo/base.h"

using namespace Interface::NDO;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy constructor.
 *
 *  Base is not copyable. Any attempt to use the copy constructor will result
 *  in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] base Unused.
 */
Base::Base(const Base& base) : stream_(NULL)
{
  (void)base;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Base is not copyable. Any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] base Unused.
 *
 *  \return *this
 */
Base& Base::operator=(const Base& base)
{
  (void)base;
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
 *  \param[in] stream Stream on which NDO protocol will act.
 */
Base::Base(IO::Stream* stream) : stream_(stream) {}

/**
 *  Destructor.
 */
Base::~Base() {}
