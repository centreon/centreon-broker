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

#include "com/centreon/broker/io/raw.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
raw::raw() {}

/**
 *  Copy constructor.
 *
 *  @param[in] r Object to copy.
 */
raw::raw(raw const& r) : io::data(r), QByteArray(r) {}

/**
 *  Destructor.
 */
raw::~raw() {}

/**
 *  Assignment operator.
 *
 *  @param[in] r Object to copy.
 *
 *  @return This object.
 */
raw& raw::operator=(raw const& r) {
  data::operator=(r);
  QByteArray::operator=(r);
  return (*this);
}

/**
 *  Get the data type.
 *
 *  @return Raw data.
 */
QString const& raw::type() const {
  static QString const raw_type("com::centreon::broker::io::raw");
  return (raw_type);
}
