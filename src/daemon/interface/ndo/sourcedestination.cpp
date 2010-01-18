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
#include <stdlib.h>                          // for abort
#include "interface/ndo/sourcedestination.h"

using namespace Interface::NDO;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy constructor.
 *
 *  As SourceDestination is not copyable, any attempt to use the copy
 *  constructor will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] sd Unused.
 */
SourceDestination::SourceDestination(const SourceDestination& sd)
  : Interface::Source(),
    Interface::Destination(),
    Interface::SourceDestination(),
    Source(NULL),
    Destination(NULL)
{
  (void)sd;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  As SourceDestination is not copyable, any attempt to use the assignment
 *  operator will result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] sd Unused.
 *
 *  \return *this
 */
SourceDestination& SourceDestination::operator=(const SourceDestination& sd)
{
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
 *  \param[in] source      Source stream.
 *  \param[in] destination Destination stream.
 */
SourceDestination::SourceDestination(IO::Stream* source,
                                     IO::Stream* destination)
  : Source(source), Destination(destination) {}

/**
 *  Destructor.
 */
SourceDestination::~SourceDestination() {}

/**
 *  Close the underlying streams.
 */
void SourceDestination::Close()
{
  this->Source::Close();
  this->Destination::Close();
  return ;
}
