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

#include "io/istream.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
istream::istream() {}

/**
 *  Copy constructor.
 *
 *  @param[in] i Object to copy.
 */
istream::istream(istream const& i) : _from(i._from) {}

/**
 *  Destructor.
 */
istream::~istream() {}

/**
 *  Assignment operator.
 *
 *  @param[in] i Object to copy.
 *
 *  @return This object.
 */
istream& istream::operator=(istream const& i) {
  _from = i._from;
  return (*this);
}

/**
 *  Set the stream from which this stream will read.
 *
 *  @param[in] from Stream to read from.
 */
void istream::read_from(QSharedPointer<istream> from) {
  _from = from;
  return ;
}
