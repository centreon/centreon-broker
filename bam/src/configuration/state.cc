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
 *  Constructor
 *
 *
 */
state::state(){
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
 *  @return  a const list of kpis
 */
state::kpis const& state::get_kpis()const{ 
  return _kpis;
}

/**
 *  get all the bool expressions
 *
 *  @return  const list of expressions
 */
state::bool_exps const& state::get_boolexps()const{ 
  return _bool_expressions; 
}



/**
 *  get all the business activities
 *
 *  @return  the list of all the business activities
 */
state::bas & state::get_bas(){ 
  return _bas; 
}

/**
 *  get_kpis
 *
 *  @return  a list of kpis
 */
state::kpis & state::get_kpis(){ 
  return _kpis;
}

/**
 *  get_boolexps 
 *
 *  @return  list of expressions
 */
state::bool_exps& state::get_boolexps(){ 
  return _bool_expressions; 
}

