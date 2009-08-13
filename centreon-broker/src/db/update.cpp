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

#include "db/update.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief Update default constructor.
 *
 *  Initialize members to their default values.
 */
Update::Update() {}

/**
 *  \brief Update copy constructor.
 *
 *  Build a new Update object by copying data from the given object.
 *
 *  \param[in] update Object to copy data from.
 */
Update::Update(const Update& update)
  : HaveArgs(update),
    HaveFields(update),
    HavePredicate(update),
    Query(update),
    HaveTable(update) {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data from the given object to the current instance.
 *
 *  \param[in] update Object to copy data from.
 *
 *  \return *this
 */
Update& Update::operator=(const Update& update)
{
  this->HaveFields::operator=(update);
  this->HavePredicate::operator=(update);
  this->HaveTable::operator=(update);
  this->Query::operator=(update);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Update destructor.
 *
 *  Release acquired ressources.
 */
Update::~Update() {}
