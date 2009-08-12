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

#include "db/have_args.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief HaveArgs default constructor.
 *
 *  Initialize the new object.
 */
HaveArgs::HaveArgs() throw () {}

/**
 *  \brief HaveArgs copy constructor.
 *
 *  Initialize the new object from the given object.
 *
 *  \param[in] ha Object to copy data from.
 */
HaveArgs::HaveArgs(const HaveArgs& ha) throw ()
{
  (void)ha;
}

/**
 *  \brief HaveArgs destructor.
 *
 *  Release acquired ressources.
 */
HaveArgs::~HaveArgs() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data of the given object to the current instance. As users shouldn't
 *  directly use this method, it is declared protected.
 *
 *  \param[in] ha Object to copy data from.
 *
 *  \return *this
 */
HaveArgs& HaveArgs::operator=(const HaveArgs& ha) throw ()
{
  (void)ha;
  return (*this);
}
