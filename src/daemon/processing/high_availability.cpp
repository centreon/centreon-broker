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
#include <stdlib.h>                       // for abort
#include "processing/high_availability.h"

using namespace Processing;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief HighAvailability copy constructor.
 *
 *  HighAvailability is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] ha Unused.
 */
HighAvailability::HighAvailability(const HighAvailability& ha)
{
  (void)ha;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  HighAvailability is not copyable. Therefore any attempt to use the
 *  assignment operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] ha Unused.
 *
 *  \return *this
 */
HighAvailability& HighAvailability::operator=(const HighAvailability& ha)
{
  (void)ha;
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
 *  HighAvailability default constructor.
 */
HighAvailability::HighAvailability() : init_(false) {}

/**
 *  HighAvailability destructor.
 */
HighAvailability::~HighAvailability()
{
  // XXX
}

/**
 *  HighAvailability thread entry point.
 */
void HighAvailability::operator()()
{
  // XXX
}

/**
 *  Initialize the HighAvailability thread.
 */
void HighAvailability::Init(Interface::Destination* destination)
{
  // XXX
}
