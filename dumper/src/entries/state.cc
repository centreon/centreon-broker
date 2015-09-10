/*
** Copyright 2015 Centreon
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** For more information : contact@centreon.com
*/

#include "com/centreon/broker/dumper/entries/ba.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/host.hh"
#include "com/centreon/broker/dumper/entries/service.hh"
#include "com/centreon/broker/dumper/entries/boolean.hh"
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
 *  Get the boolean rules.
 *
 *  @return  Non-modifiable list of boolean rules.
 */
std::list<boolean> const& state::get_booleans() const {
  return (_booleans);
}

/**
 *  Get the boolean rules.
 *
 *  @return  Modifiable list of boolean rules.
 */
std::list<boolean>& state::get_booleans() {
  return (_booleans);
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
  _booleans = other._booleans;
  _kpis = other._kpis;
  _hosts = other._hosts;
  _services = other._services;
  return ;
}
