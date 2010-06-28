/*
** This file is part of Centreon Dhana.
**
** Centreon Dhana is free software: you can redistribute it and/or modify it
** under the terms of the GNU Affero General Public License as published by the
** Free Software Foundation, either version 3 of the License, or (at your
** option) any later version.
**
** Centreon Dhana is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public
** License for more details.
**
** You should have received a copy of the GNU Affero General Public License
** along with Centreon Dhana. If not, see <http://www.gnu.org/licenses/>.
*/

#include "logging/void_logger.hh"

using namespace logging;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
void_logger::void_logger() {}

/**
 *  Copy constructor.
 *
 *  @param[in] v Unused.
 */
void_logger::void_logger(void_logger const& v)
{
  (void)v;
}

/**
 *  Destructor.
 */
void_logger::~void_logger() {}

/**
 *  Assignment operator overload.
 *
 *  @param[in] v Unused.
 *
 *  @return Current instance.
 */
void_logger& void_logger::operator=(void_logger const& v)
{
  (void)v;
  return (*this);
}
