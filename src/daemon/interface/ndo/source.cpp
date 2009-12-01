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
#include <stdlib.h>                 // for abort
#include "interface/ndo/internal.h"
#include "interface/ndo/source.h"
#include "io/stream.h"

using namespace Interface::NDO;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Source copy constructor.
 *
 *  As Source is not copyable, any attempt to use the copy constructor will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] source Unused.
 */
Source::Source(const Source& source) : Interface::Source(source)
{
  (void)source;
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Source is not copyable, any attempt to use the assignment operator will
 *  result in a call to abort().
 *  \par Safety No exception safety.
 *
 *  \param[in] source Unused.
 *
 *  \return *this
 */
Source& Source::operator=(const Source& source)
{
  (void)source;
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
 *  \brief Source constructor.
 *
 *  Build an NDO input source that uses the stream object as raw binary input.
 *  The stream object must not be NULL and is owned by the Source object upon
 *  successful return from the constructor.
 *
 *  \param[in] stream Input stream object.
 */
Source::Source(IO::Stream* stream) : stream_(stream) {}

/**
 *  Source destructor.
 */
Source::~Source() {}

/**
 *  Close the source object.
 */
void Source::Close()
{
  this->stream_->Close();
  return ;
}

/**
 *  \brief Get the next available event.
 *
 *  Extract the next available event on the input stream.
 *
 *  \return Next available event.
 */
Events::Event* Source::Event()
{
}
