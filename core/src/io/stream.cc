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

#include "com/centreon/broker/io/stream.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
stream::stream() {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
stream::stream(stream const& s) {
  _from = s._from;
  _to = s._to;
}

/**
 *  Destructor.
 */
stream::~stream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
stream& stream::operator=(stream const& s) {
  if (&s != this) {
    _from = s._from;
    _to = s._to;
  }
  return (*this);
}

/**
 *  Read from another stream.
 *
 *  @param[in] from Stream to read from.
 */
void stream::read_from(QSharedPointer<stream> from) {
  _from = from;
  return ;
}

/**
 *  Write to another stream.
 *
 *  @param[in] to Stream to write to.
 */
void stream::write_to(QSharedPointer<stream> to) {
  _to = to;
  return ;
}
