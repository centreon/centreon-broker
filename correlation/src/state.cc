/*
** Copyright 2009-2011 Merethis
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

#include "com/centreon/broker/correlation/state.hh"

using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members from the given object.
 *
 *  @param[in] s Object to copy.
 */
void state::_internal_copy(state const& s) {
  current_state = s.current_state;
  end_time = s.end_time;
  host_id = s.host_id;
  service_id = s.service_id;
  start_time = s.start_time;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
state::state()
  : current_state(-1),
    end_time(0),
    host_id(0),
    service_id(0),
    start_time(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
state::state(state const& s) : io::data(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& s) {
  io::data::operator=(s);
  _internal_copy(s);
  return (*this);
}
