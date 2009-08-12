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

#include "db/select.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief Select copy constructor.
 *
 *  Copy data of the given object to the current instance.
 *
 *  \param[in] select Object to copy data from.
 */
Select::Select(const Select& select)
  : HaveFields(select), HavePredicate(select), HaveTable(select), Query(select)
{}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data of the given object to the current instance.
 *
 *  \param[in] select Object to copy data from.
 *
 *  \return *this
 */
Select& Select::Select(const Select& select)
{
  this->HaveFields::operator=(select);
  this->HavePredicate::operator=(select);
  this->HaveTable::operator=(select);
  this->Query::operator=(select);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Select default constructor.
 *
 *  Initialize members to their default values.
 */
Select::Select() {}

/**
 *  \brief Select destructor.
 *
 *  Release all acquired ressources.
 */
Select::~Select() {}
