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
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
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

/**
 *  Get Hosts.
 *
 *  @return Non-modifiable list of hosts.
 */
std::list<host> const& state::get_hosts() const {
  return (_hosts);
}

/**
 *  Get hosts.
 *
 *  @return Modifiable list of hosts.
 */
std::list<host>& state::get_hosts() {
  return (_hosts);
}

/**
 *  Get services.
 *
 *  @return Non-modifiable list of service.
 */
std::list<service> const& state::get_services() const {
  return (_services);
}

/**
 *  Get services.
 *
 *  @return Modifiable list of service.
 */
std::list<service>& state::get_services() {
  return (_services);
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
  _hosts = other._hosts;
  _services = other._services;
  return ;
}
