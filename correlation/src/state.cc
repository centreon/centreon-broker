/*
** Copyright 2009-2013 Centreon
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

#include "com/centreon/broker/correlation/state.hh"

using namespace com::centreon::broker::correlation;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Constructor.
 */
state::state()
  : ack_time(-1),
    current_state(3),
    end_time(0),
    host_id(0),
    instance_id(0),
    in_downtime(false),
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
  ack_time = s.ack_time;
  current_state = s.current_state;
  end_time = s.end_time;
  host_id = s.host_id;
  instance_id = s.instance_id;
  in_downtime = s.in_downtime;
  service_id = s.service_id;
  start_time = s.start_time;
  return ;
}
