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
#include <stdlib.h>                    // for abort
#include "interface/xml/destination.h"
#include "io/stream.h"

using namespace Interface::XML;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Destination copy constructor.
 *
 *  Destination is not copyable. Therefore any attempt to use the copy
 *  constructor will result in a call to abort.
 *
 *  \param[in] dest Unused.
 */
Destination::Destination(const Destination& dest)
  : Interface::Destination(dest)
{
  assert(false);
  abort();
}

/**
 *  \brief Assignment operator overload.
 *
 *  Destination is not copyable. Therefore any attempt to use the assignment
 *  operator will result in a call to abort.
 *
 *  \param[in] dest Unused.
 *
 *  \return *this
 */
Destination& Destination::operator=(const Destination& dest)
{
  (void)dest;
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
 *  Destination constructor.
 *
 *  \param[in] stream Stream on which data will be sent.
 */
Destination::Destination(IO::Stream* stream) : stream_(stream) {}

/**
 *  Destination destructor.
 */
Destination::~Destination() {}

/**
 *  Close the underlying stream.
 */
void Destination::Close()
{
  this->stream_->Close();
  return ;
}

/**
 *  Dump event to the stream.
 *
 *  \param[in] event Event to dump.
 */
void Destination::Event(const Events::Event& event)
{
}
