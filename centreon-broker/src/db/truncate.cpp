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

#include "db/truncate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*         Protected Methods           *
*                                     *
**************************************/

/**
 *  \brief Truncate default constructor.
 *
 *  Initialize members to their default values.
 */
Truncate::Truncate() {}

/**
 *  \brief Truncate copy constructor.
 *
 *  Construct the current instance by copying data from the given object.
 *
 *  \param[in] truncate Object to copy data from.
 */
Truncate::Truncate(const Truncate& truncate)
  : HaveTable(truncate), Query(truncate) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy all data of the given object to the current instance.
 *
 *  \param[in] truncate Object to copy data from.
 *
 *  \return *this
 */
Truncate& Truncate::operator=(const Truncate& truncate)
{
  this->HaveTable::operator=(truncate);
  this->Query::operator=(truncate);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Truncate destructor.
 *
 *  Release previously allocated ressources.
 */
Truncate::~Truncate() {}
