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

#include "interface/destination/destination.h"

using namespace Interface::Destination;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Destination default constructor.
 */
Destination::Destination() {}

/**
 *  \brief Destination copy constructor.
 *
 *  As Destination is an interface object with no data members, the copy
 *  constructor does nothing.
 *
 *  \param[in] destination Unused.
 */
Destination::Destination(const Destination& destination)
{
  (void)destination;
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Destination is an interface object with no data members, the assignment
 *  operator does nothing.
 *
 *  \param[in] destination Unused.
 *
 *  \return *this
 */
Destination& Destination::operator=(const Destination& destination)
{
  (void)destination;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Destination destructor.
 */
Destination::~Destination() {}
