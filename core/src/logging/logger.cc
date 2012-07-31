/*
** Copyright 2009-2011 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
*/

#include <cassert>
#include <cstdlib>
#include "com/centreon/broker/logging/logger.hh"
#include "com/centreon/broker/logging/manager.hh"

using namespace com::centreon::broker::logging;

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

/**
 *  Get temporary logging object.
 *
 *  @param[in] l Log level.
 *
 *  @return Temporary logging object.
 */
temp_logger logger::operator()(level l) throw () {
  return (manager::instance().get_temp_logger(_type, l));
}

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
logger::logger(logger const& l) {
  _internal_copy(l);
}

/**
 *  @brief Assignment operator.
 *
 *  Logger objects cannot be copied.
 *
 *  @param[in] l Unused.
 *
 *  @return This object.
 */
logger& logger::operator=(logger const& l) {
  _internal_copy(l);
  return (*this);
}

/**
 *  Calls abort().
 *
 *  @param[in] l Unused.
 */
void logger::_internal_copy(logger const& l) {
  (void)l;
  assert(!"logger is not copyable");
  abort();
  return ;
}
