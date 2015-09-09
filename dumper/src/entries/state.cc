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
#include "com/centreon/broker/dumper/entries/ba_type.hh"
#include "com/centreon/broker/dumper/entries/kpi.hh"
#include "com/centreon/broker/dumper/entries/organization.hh"
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

/**
 *  Get organizations.
 *
 *  @return Non-modifiable list of organizations.
 */
std::list<organization> const& state::get_organizations() const {
  return (_organizations);
}

/**
 *  Get organizations.
 *
 *  @return Modifiable list of organizations.
 */
std::list<organization>& state::get_organizations() {
  return (_organizations);
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
  _organizations = other._organizations;
  return ;
}
