/*
** Copyright 2011 Merethis
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

#include "com/centreon/broker/io/data.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
data::data() : instance_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] d Object to copy.
 */
data::data(data const& d) : instance_id(d.instance_id) {}

/**
 *  Destructor.
 */
data::~data() {}

/**
 *  Assignment operator.
 *
 *  @param[in] d Object to copy.
 *
 *  @return This object.
 */
data& data::operator=(data const& d) {
  instance_id = d.instance_id;
  return (*this);
}
