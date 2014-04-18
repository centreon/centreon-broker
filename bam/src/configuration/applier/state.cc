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
** <http:/www.gnu.org/licenses/>.
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
  _ba_applier.apply(my_state.get_bas());
  _kpi_applier.apply(my_state.get_kpis(), _ba_applier);
  _bool_exp_applier.apply(my_state.get_bool_exps(), _ba_applier);
  return ;
}
