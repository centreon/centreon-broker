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

#include <cassert>
#include "db/have_fields.h"

using namespace CentreonBroker::DB;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  HaveFields copy constructor. No reason we would want to copy this
 *  class so declare it private.
 */
HaveFields::HaveFields(const HaveFields& hf) throw ()
{
  (void)hf;
  assert(false);
}

/**
 *  HaveFields operator= overload. No reason we would want to copy this
 *  class so declare it private.
 */
HaveFields& HaveFields::operator=(const HaveFields& hf) throw ()
{
  (void)hf;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  HaveFields constructor.
 */
HaveFields::HaveFields() throw ()
{
}

/**
 *  HaveFields destructor.
 */
HaveFields::~HaveFields()
{
}

