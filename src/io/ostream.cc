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

#include "io/ostream.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
ostream::ostream() {}

/**
 *  Copy constructor.
 *
 *  @param[in] o Object to copy.
 */
ostream::ostream(ostream const& o) : _to(o._to) {}

/**
 *  Destructor.
 */
ostream::~ostream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] o Object to copy.
 *
 *  @return This object.
 */
ostream& ostream::operator=(ostream const& o) {
  _to = o._to;
  return (*this);
}

/**
 *  Set the stream to which this stream will write.
 *
 *  @param[in] to Stream to write to.
 */
void ostream::write_to(QSharedPointer<ostream> to) {
  _to = to;
  return ;
}
