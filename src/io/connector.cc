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

#include "io/connector.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
connector::connector() {}

/**
 *  Copy constructor.
 *
 *  @param[in] c Object to copy.
 */
connector::connector(connector const& c) : _down(c._down) {}

/**
 *  Destructor.
 */
connector::~connector() {}

/**
 *  Assignment operator.
 *
 *  @param[in] c Object to copy.
 *
 *  @return This object.
 */
connector& connector::operator=(connector const& c) {
  _down = c._down;
  return (*this);
}

/**
 *  Set the object to forward successful connection to.
 *
 *  @param[in] down Object that will receive new connections.
 */
void connector::on(QSharedPointer<connector> down) {
  _down = down;
  return ;
}
