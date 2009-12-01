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

#include "interface/source.h"

using namespace Interface;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Source default constructor.
 */
Source::Source() {}

/**
 *  \brief Source copy constructor.
 *
 *  As Source is an interface object with no data members, the copy constructor
 *  does nothing.
 *
 *  \param[in] source Unused.
 */
Source::Source(const Source& source)
{
  (void)source;
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Source is an interface object with no data members, the assignment
 *  operator does nothing.
 *  \par Safety No throw guarantee.
 *
 *  \param[in] source Unused.
 *
 *  \return *this
 */
Source& Source::operator=(const Source& source)
{
  (void)source;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Source destructor.
 */
Source::~Source() {}
