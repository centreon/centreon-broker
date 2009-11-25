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

#include "io/acceptor.h"

using namespace IO;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  Acceptor default constructor.
 */
Acceptor::Acceptor() {}

/**
 *  \brief Acceptor copy constructor.
 *
 *  As Acceptor does not hold any data member, this constructor does nothing.
 *
 *  \param[in] acceptor Unused.
 */
Acceptor::Acceptor(const Acceptor& acceptor)
{
  (void)acceptor;
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  As Acceptor does not hold any data member, this method does nothing.
 *
 *  \param[in] acceptor Unused.
 *
 *  \return *this
 */
Acceptor& Acceptor::operator=(const Acceptor& acceptor)
{
  (void)acceptor;
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Acceptor destructor.
 */
Acceptor::~Acceptor() {}
