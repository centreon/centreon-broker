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

#include "correlation/correlator.h"

using namespace Correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  \brief Copy internal members.
 *
 *  This method is used by the copy constructor and the assignment operator.
 *
 *  \param[in] correlator Object to copy.
 */
void Correlator::InternalCopy(const Correlator& correlator)
{
  this->hosts_    = correlator.hosts_;
  this->issues_   = correlator.issues_;
  this->services_ = correlator.services_;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
Correlator::Correlator() {}

/**
 *  Copy constructor.
 *
 *  \param[in] correlator Object to copy.
 */
Correlator::Correlator(const Correlator& correlator)
{
  this->InternalCopy(correlator);
}

/**
 *  Destructor.
 */
Correlator::~Correlator() {}

/**
 *  Assignment operator overload.
 *
 *  \param[in] correlator Object to copy.
 *
 *  \return *this
 */
Correlator& Correlator::operator=(const Correlator& correlator)
{
  this->InternalCopy(correlator);
  return (*this);
}

/**
 *  Treat a new event.
 *
 *  \param[inout] event Event to process.
 */
void Correlator::Event(Events::Event& event)
{
}
