/*
** Copyright 2014 Merethis
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

#include "com/centreon/broker/bam/configuration/state.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 */
state::state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
state::state(state const& other)
  : _bas(other._bas),
    _kpis(other._kpis),
    _bool_expressions(other._bool_expressions),
    _mapping(other._mapping),
    _meta_services(other._meta_services),
    _timeperiods(other._timeperiods) {}

/**
 *  Destructor
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
  if (this != &other) {
    _bas = other._bas;
    _kpis= other._kpis;
    _bool_expressions = other._bool_expressions;
    _mapping = other._mapping;
    _meta_services = other._meta_services;
    _timeperiods = other._timeperiods;
  }
  return (*this);
}

/**
 *  Clear state.
 */
void state::clear() {
  _bas.clear();
  _kpis.clear();
  _bool_expressions.clear();
  _meta_services.clear();
  _timeperiods.clear();
  return ;
}

/**
 *  Get the list of BAs.
 *
 *  @return  A const list of all the business activities.
 */
state::bas const& state::get_bas() const {
  return (_bas);
}

/**
 *  Get all the kpis.
 *
 *  @return  A const list of kpis.
 */
state::kpis const& state::get_kpis() const {
  return (_kpis);
}

/**
 *  Get all the boolean expressions.
 *
 *  @return  A list of constant expressions.
 */
state::bool_exps const& state::get_bool_exps() const {
  return (_bool_expressions);
}

/**
 *  Get meta-services.
 *
 *  @return  A list of meta-services.
 */
state::meta_services const& state::get_meta_services() const {
  return (_meta_services);
}

/**
 *  Get mapping.
 *
 *  @return Mapping.
 */
bam::hst_svc_mapping const& state::get_mapping() const {
  return (_mapping);
}

/**
 *  Get timeperiods.
 *
 *  @return  Timeperiods.
 */
state::timeperiods const& state::get_timeperiods() const {
  return (_timeperiods);
}

/**
 *  Get all the business activities
 *
 *  @return  The list of all the business activities.
 */
state::bas & state::get_bas() {
  return (_bas);
}

/**
 *  Get all the kpis
 *
 *  @return  A list of kpis.
 */
state::kpis & state::get_kpis() {
  return (_kpis);
}

/**
 *  Get all the boolean expressions.
 *
 *  @return  A list of expressions.
 */
state::bool_exps& state::get_bool_exps() {
  return (_bool_expressions);
}

/**
 *  Get all the meta-services.
 *
 *  @return  A list of meta-services.
 */
state::meta_services& state::get_meta_services() {
  return (_meta_services);
}

/**
 *  Get mapping.
 *
 *  @return Mapping.
 */
bam::hst_svc_mapping& state::get_mapping() {
  return (_mapping);
}

/**
 *  Get timeperiods.
 *
 *  @return  Timeperiods.
 */
state::timeperiods& state::get_timeperiods() {
  return (_timeperiods);
}
