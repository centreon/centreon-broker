/*
** Copyright 2011-2012 Merethis
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
#include <iostream>
#include "com/centreon/broker/io/endpoint.hh"

using namespace com::centreon::broker::io;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 *
 *  @param[in] is_accptr true if endpoint is an acceptor.
 */
endpoint::endpoint(bool is_accptr) : _is_acceptor(is_accptr) {}

/**
 *  Copy constructor.
 *
 *  @param[in] e Object to copy.
 */
endpoint::endpoint(endpoint const& e) {
  _internal_copy(e);
}

/**
 *  Destructor.
 */
endpoint::~endpoint() {}

/**
 *  Assignment operator.
 *
 *  @param[in] e Object to copy.
 *
 *  @return This object.
 */
endpoint& endpoint::operator=(endpoint const& e) {
  if (this != &e)
    _internal_copy(e);
  return (*this);
}

/**
 *  Set the lower layer endpoint object of this endpoint.
 *
 *  @param[in] endp Lower layer endpoint object.
 */
void endpoint::from(misc::shared_ptr<endpoint> endp) {
  _from = endp;
  return ;
}

/**
 *  Check if this endpoint is an acceptor.
 *
 *  @return true if endpoint is an acceptor.
 */
bool endpoint::is_acceptor() const throw () {
  return (_is_acceptor);
}

/**
 *  Check if this endpoint is a connector.
 *
 *  @return true if endpoint is a connector.
 */
bool endpoint::is_connector() const throw () {
  return (!_is_acceptor);
}

/**
 *  Generate statistics about the endpoint.
 *
 *  @param[out] buffer Unused.
 */
void endpoint::stats(std::string& buffer) {
  if (!_from.isNull())
    _from->stats(buffer);
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] e Object to copy.
 */
void endpoint::_internal_copy(endpoint const& e) {
  _from = e._from;
  _is_acceptor = e._is_acceptor;
  return ;
}
