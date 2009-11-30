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

#include "interface/sourcedestination.h"

using namespace Interface;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  SourceDestination default constructor.
 */
SourceDestination::SourceDestination() {}

/**
 *  \brief SourceDestination copy constructor.
 *
 *  As SourceDestination does not hold any data member, this copy constructor
 *  does nothing much than the default constructor.
 *
 *  \param[in] sd Unused.
 *
 *  \see SourceDestination::SourceDestination()
 */
SourceDestination::SourceDestination(const SourceDestination& sd)
  : Source::Source(sd), Destination::Destination(sd) {}

/**
 *  \brief Assignment operator overload.
 *
 *  As SourceDestination does not hold any data member, this copy constructor
 *  does nothing.
 *
 *  \param[in] sd Unused.
 *
 *  \return *this
 */
SourceDestination& SourceDestination::operator=(const SourceDestination& sd)
{
  (void)sd;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  SourceDestination destructor.
 */
SourceDestination::~SourceDestination() {}
