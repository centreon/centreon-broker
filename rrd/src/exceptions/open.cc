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

#include "com/centreon/broker/rrd/exceptions/open.hh"

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
open::open() throw () {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
open::open(open const& o) throw ()
  : broker::exceptions::msg(o) {}

/**
 *  Destructor.
 */
open::~open() throw () {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
open& open::operator=(open const& o) throw () {
  broker::exceptions::msg::operator=(o);
  return (*this);
}

/**
 *  Clone the exception object.
 *
 *  @return Copy of this exception object.
 */
exceptions::msg* open::clone() const {
  return (new open(*this));
}

/**
 *  Rethrow the exception.
 */
void open::rethrow() const {
  throw (*this);
  return ;
}
