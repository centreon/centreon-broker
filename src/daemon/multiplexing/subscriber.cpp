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

#include "multiplexing/subscriber.h"

using namespace Multiplexing;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Subscriber default constructor.
 */
Subscriber::Subscriber() {}

/**
 *  \brief Subscriber copy constructor.
 *
 *  As Subscriber does not hold any data member, no action is performed.
 *
 *  \param[in] subscriber Unused.
 */
Subscriber::Subscriber(const Subscriber& subscriber)
{
  (void)subscriber;
}

/**
 *  \brief Assignment operator overload.
 *
 *  As Subscriber does not hold any data member, no action is performed.
 *
 *  \param[in] subscriber Unused.
 *
 *  \return *this
 */
Subscriber& Subscriber::operator=(const Subscriber& subscriber)
{
  (void)subscriber;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Subscriber destructor.
 */
Subscriber::~Subscriber() {}
