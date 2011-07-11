/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/rrd/exceptions/update.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::rrd::exceptions;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
update::update() throw () {}

/**
 *  Copy constructor.
 *
 *  @param[in] u Object to copy.
 */
update::update(update const& u) throw ()
  : broker::exceptions::msg(u) {}

/**
 *  Destructor.
 */
update::~update() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] u Object to copy.
 *
 *  @return This object.
 */
update& update::operator=(update const& u) throw () {
  broker::exceptions::msg::operator=(u);
  return (*this);
}

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
exceptions::msg* update::clone() const {
  return (new update(*this));
}

/**
 *  Rethrow the exception.
 */
void update::rethrow() const {
  throw (*this);
  return ;
}
