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

#include "io/acceptor.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
acceptor::acceptor() {}

/**
 *  Copy constructor.
 *
 *  @param[in] a Object to copy.
 */
acceptor::acceptor(acceptor const& a) : endpoint(a), _down(a._down) {}

/**
 *  Destructor.
 */
acceptor::~acceptor() {}

/**
 *  Assignment operator.
 *
 *  @param[in] a Object to copy.
 *
 *  @return This object.
 */
acceptor& acceptor::operator=(acceptor const& a) {
  endpoint::operator=(a);
  _down = a._down;
  return (*this);
}

/**
 *  Forward incoming object to the following acceptor.
 *
 *  @param[in] down Object to forward to.
 */
void acceptor::from(QSharedPointer<acceptor> down) {
  _down = down;
  return ;
}
