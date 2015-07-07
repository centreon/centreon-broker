/*
** Copyright 2015 Merethis
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

#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/state.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::dumper::entries;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
state::state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
state::state(state const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void state::_internal_copy(state const& other) {
  _bas = other._bas;
  _kpis = other._kpis;
  return ;
}
