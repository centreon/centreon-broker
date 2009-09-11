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
HavePredicate::HavePredicate() throw () {}

/**
 *  \brief HavePredicate copy constructor.
 *
 *  Build the new instance by copying the predicate from the given object.
 *
 *  \param[in] hp Object to copy the predicate from.
 */
HavePredicate::HavePredicate(const HavePredicate& hp) : HaveArgs(hp)
{
  if (hp.predicate.get())
    this->predicate.reset(hp.predicate->Duplicate());
}

/**
 *  \brief HavePredicate destructor.
 *
 *  Release previously acquired ressources.
 */
HavePredicate::~HavePredicate() {}

/**
 *  \brief Overload of the assignment operator.
 *
 *  Copy the predicate of the given object to the current instance.
 *
 *  \param[in] hp Object to copy the predicate from.
 *
 *  \return *this
 */
HavePredicate& HavePredicate::operator=(const HavePredicate& hp)
{
  this->HaveArgs::operator=(hp);
  if (hp.predicate.get())
    this->predicate.reset(hp.predicate->Duplicate());
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  \brief Set the query predicate.
 *
 *  The predicate argument will be duplicated within the object.
 *
 *  \param[in] pred Query predicate.
 */
void HavePredicate::SetPredicate(const Predicate& pred)
{
  this->predicate.reset(pred.Duplicate());
  return ;
}
