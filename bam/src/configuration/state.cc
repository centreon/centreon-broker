/*
** Copyright 2014-2015 Centreon
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
    : _ba_svc_mapping(other._ba_svc_mapping),
      _bas(other._bas),
      _kpis(other._kpis),
      _bool_expressions(other._bool_expressions),
      _hst_svc_mapping(other._hst_svc_mapping),
      _meta_svc_mapping(other._meta_svc_mapping),
      _meta_services(other._meta_services) {}

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
    _ba_svc_mapping = other._ba_svc_mapping;
    _bas = other._bas;
    _kpis = other._kpis;
    _bool_expressions = other._bool_expressions;
    _hst_svc_mapping = other._hst_svc_mapping;
    _meta_svc_mapping = other._meta_svc_mapping;
    _meta_services = other._meta_services;
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
  return;
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
 *  Get host/service mapping.
 *
 *  @return Mapping.
 */
bam::hst_svc_mapping const& state::get_hst_svc_mapping() const {
  return (_hst_svc_mapping);
}

/**
 *  Get BA/service mapping.
 *
 *  @return Mapping.
 */
bam::ba_svc_mapping const& state::get_ba_svc_mapping() const {
  return (_ba_svc_mapping);
}

/**
 *  Get meta-service/service mapping.
 *
 *  @return Mapping.
 */
bam::ba_svc_mapping const& state::get_meta_svc_mapping() const {
  return (_meta_svc_mapping);
}

/**
 *  Get all the business activities
 *
 *  @return  The list of all the business activities.
 */
state::bas& state::get_bas() {
  return (_bas);
}

/**
 *  Get all the kpis
 *
 *  @return  A list of kpis.
 */
state::kpis& state::get_kpis() {
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
 *  Get host/service mapping.
 *
 *  @return Mapping.
 */
bam::hst_svc_mapping& state::get_hst_svc_mapping() {
  return (_hst_svc_mapping);
}

/**
 *  Get BA/service mapping.
 *
 *  @return Mapping.
 */
bam::ba_svc_mapping& state::get_ba_svc_mapping() {
  return (_ba_svc_mapping);
}

/**
 *  Get meta-service/service mapping.
 *
 *  @return Mapping.
 */
bam::ba_svc_mapping& state::get_meta_svc_mapping() {
  return (_meta_svc_mapping);
}
