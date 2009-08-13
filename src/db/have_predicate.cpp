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

#include "db/have_predicate.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*          Protected Methods          *
*                                     *
**************************************/

/**
 *  \brief HavePredicate default constructor.
 *
 *  Initialize members to their default values.
 */
HavePredicate::HavePredicate() throw () : predicate(NULL) {}

/**
 *  \brief HavePredicate copy constructor.
 *
 *  Build the new instance by copying data from the given object.
 *
 *  \param[in] hp Object to copy data from.
 */
HavePredicate::HavePredicate(const HavePredicate& hp) : HaveArgs(hp)
{
  // XXX : copy predicate
}

/**
 *  \brief HavePredicate destructor.
 *
 *  Release previously acquired ressources.
 */
HavePredicate::~HavePredicate()
{
  if (this->predicate)
    delete (this->predicate);
}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy data of the given object to the current instance.
 *
 *  \param[in] hp Object to copy data from.
 *
 *  \return *this
 */
HavePredicate& HavePredicate::operator=(const HavePredicate& hp)
{
  this->HaveArgs::operator=(hp);
  // XXX : copy predicate
  return (*this);
}
