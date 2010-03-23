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
#include <stdlib.h>               // for abort
#include "interface/xml/source.h"

using namespace Interface::XML;

/**************************************
*                                     *
*           Static Methods            *
*                                     *
**************************************/

/**
 *  Extract event parameters from the data stream.
 */
template <typename T>
static T* HandleEvent()
{
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Source copy constructor.
 *
 *  Source is not copyable. Therefore any attempt to use the copy constructor
 *  will result in a call to abort().
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
 *  Source is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort().
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
 *  Source constructor.
 *
 *  \param[in] stream Stream on which data will be sent.
 */
Source::Source(IO::Stream* stream) : stream_(stream) {}

/**
 *  Source destructor.
 */
Source::~Source() {}

/**
 *  Close the underlying stream.
 */
void Source::Close()
{
  this->stream_->Close();
  return ;
}

/**
 *  \brief Get the next available event.
 *
 *  Extract the next available event on the input stream, NULL if the stream is
 *  closed.
 *
 *  \return Next available event, NULL if stream is closed.
 */
Events::Event* Source::Event()
{
}
