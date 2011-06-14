/*
** Copyright 2009-2011 MERETHIS
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

#include "logging/void_logger.hh"

using namespace com::centreon::broker::logging;

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
void_logger::void_logger(void_logger const& v) {
  (void)v;
}

/**
 *  Destructor.
 */
void_logger::~void_logger() {}

/**
 *  Assignment operator.
 *
 *  @param[in] v Unused.
 *
 *  @return This object.
 */
void_logger& void_logger::operator=(void_logger const& v) {
  (void)v;
  return (*this);
}
