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
#include "com/centreon/broker/dumper/entries/ba_type.hh"
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

/**
 *  Get BA types.
 *
 *  @return Non-modifiable list of BA types.
 */
std::list<ba_type> const& state::get_ba_types() const {
  return (_ba_types);
}

/**
 *  Get BA types.
 *
 *  @return Modifiable list of BA types.
 */
std::list<ba_type>& state::get_ba_types() {
  return (_ba_types);
}

/**
 *  Get BAs.
 *
 *  @return Non-modifiable list of BA.
 */
std::list<ba> const& state::get_bas() const {
  return (_bas);
}

/**
 *  Get BAs.
 *
 *  @return Modifiable list of BA.
 */
std::list<ba>& state::get_bas() {
  return (_bas);
}

/**
 *  Get KPIs.
 *
 *  @return Non-modifiable list of KPI.
 */
std::list<kpi> const& state::get_kpis() const {
  return (_kpis);
}

/**
 *  Get KPIs.
 *
 *  @return Modifiable list of KPI.
 */
std::list<kpi>& state::get_kpis() {
  return (_kpis);
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
  _ba_types = other._ba_types;
  _bas = other._bas;
  _kpis = other._kpis;
  return ;
}
