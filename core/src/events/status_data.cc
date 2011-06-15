/*
** Copyright 2011 Merethis
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

#include "events/status_data.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] sd Object to copy.
 */
void status_data::_internal_copy(status_data const& sd) {
  ctime = sd.ctime;
  index_id = sd.index_id;
  interval = sd.interval;
  rrd_len = sd.rrd_len;
  status = sd.status;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
status_data::status_data()
  : ctime(0),
    index_id(0),
    interval(0),
    rrd_len(0),
    status(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] sd Object to copy.
 */
status_data::status_data(status_data const& sd) : event(sd) {
  _internal_copy(sd);
}

/**
 *  Destructor.
 */
status_data::~status_data() {}

/**
 *  Assignment operator.
 *
 *  @param[in] sd Object to copy.
 *
 *  @return This object.
 */
status_data& status_data::operator=(status_data const& sd) {
  event::operator=(sd);
  _internal_copy(sd);
  return (*this);
}

/**
 *  Get the event type (STATUSDATA).
 *
 *  @return STATUSDATA.
 */
unsigned int status_data::type() const {
  return (STATUSDATA);
}
