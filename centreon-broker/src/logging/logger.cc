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

#include <assert.h>
#include "logging/logger.hh"

using namespace logging;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy constructor.
 *
 *  Logger objects cannot be copied.
 *
 *  @param[in] l Unused.
 */
logger::logger(logger const& l)
{
  (void)l;
  assert(false);
}

/**
 *  @brief Assignment operator overload.
 *
 *  Logger objects cannot be copied.
 *
 *  @param[in] l Unused.
 *
 *  @return Current instance.
 */
logger& logger::operator=(logger const& l)
{
  (void)l;
  assert(false);
  return (*this);
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
logger::logger(type log_type) : _type(log_type) {}

/**
 *  Destructor.
 */
logger::~logger() {}
