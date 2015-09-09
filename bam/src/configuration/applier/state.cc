/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/bam/configuration/applier/state.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam::configuration;

/**
 *  Default constructor.
 */
applier::state::state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  Object to copy.
 */
applier::state::state(applier::state const& other) {
  _internal_copy(other);
}

/**
 *  Destructor.
 */
applier::state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] other  Object to copy.
 *
 *  @return This object.
 */
applier::state& applier::state::operator=(applier::state const& other) {
  if (this != &other)
    _internal_copy(other);
  return (*this);
}

/**
 *  Apply configuration.
 *
 *  @param[in] my_state  Configuration state.
 */
void applier::state::apply(bam::configuration::state const& my_state) {
  _ba_applier.apply(my_state.get_bas(), _book_service);
  _meta_service_applier.apply(
                          my_state.get_meta_services(),
                          _book_metric);
  _bool_exp_applier.apply(
                      my_state.get_bool_exps(),
                      my_state.get_hst_svc_mapping(),
                      _book_service);
  _kpi_applier.apply(
                 my_state.get_kpis(),
                 my_state.get_hst_svc_mapping(),
                 _ba_applier,
                 _meta_service_applier,
                 _bool_exp_applier,
                 _book_service);
  return ;
}

/**
 *  Get the book of metric listeners.
 *
 *  @return Book of metric listeners.
 */
bam::metric_book& applier::state::book_metric() {
  return (_book_metric);
}

/**
 *  Get the book of service listeners.
 *
 *  @return Book of service listeners.
 */
bam::service_book& applier::state::book_service() {
  return (_book_service);
}

/**
 *  @brief Visit applied state.
 *
 *  This method is used to generate default status.
 *
 *  @param[out] visitor  Visitor.
 */
void applier::state::visit(io::stream* visitor) {
  _ba_applier.visit(visitor);
  _kpi_applier.visit(visitor);
  _bool_exp_applier.visit(visitor);
  return ;
}

/**
 *  Copy internal data members.
 *
 *  @param[in] other  Object to copy.
 */
void applier::state::_internal_copy(applier::state const& other) {
  _ba_applier = other._ba_applier;
  _book_metric = other._book_metric;
  _book_service = other._book_service;
  _kpi_applier = other._kpi_applier;
  _bool_exp_applier = other._bool_exp_applier;
  _meta_service_applier = other._meta_service_applier;
  return ;
}
