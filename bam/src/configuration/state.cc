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

using namespace com::centreon::broker::bam::configuration;

/**
 *  Constructor.
 */
state::state() {}

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
state::state(state const& right)
  : _bas(right._bas),
    _kpis(right._kpis),
    _bool_expressions(right._bool_expressions) {}

/**
 *  Destructor
 */
state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& right) {
  if (this != &right) {
    _bas = right._bas;
    _kpis= right._kpis;
    _bool_expressions= right._bool_expressions;
  }
  return (*this);
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
 *  Get all the bool expressions.
 *
 *  @return  A list of constant expressions.
 */
state::bool_exps const& state::get_boolexps() const {
  return (_bool_expressions);
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
 *  Get all the boolexps
 *
 *  @return  A list of expressions.
 */
state::bool_exps& state::get_boolexps() {
  return (_bool_expressions);
}

