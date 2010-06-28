/*
**  Copyright 2010 MERETHIS
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

#include "logging/backend.hh"

using namespace logging;

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
backend::backend(backend const& b)
{
  (void)b;
}

/**
 *  Destructor.
 */
backend::~backend() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] b Unused.
 *
 *  @return Current instance.
 */
backend& backend::operator=(backend const& b)
{
  (void)b;
  return (*this);
}
