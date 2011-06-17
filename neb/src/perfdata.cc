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

#include "events/perfdata.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] p Object to copy.
 */
void perfdata::_internal_copy(perfdata const& p) {
  ctime = p.ctime;
  interval = p.interval;
  metric_id = p.metric_id;
  name = p.name;
  rrd_len = p.rrd_len;
  value = p.value;
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
perfdata::perfdata()
  : ctime(0),
    interval(0),
    metric_id(0),
    rrd_len(0),
    value(0.0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] p Object to copy.
 */
perfdata::perfdata(perfdata const& p) : event(p) {
  _internal_copy(p);
}

/**
 *  Destructor.
 */
perfdata::~perfdata() {}

/**
 *  Assignment operator.
 *
 *  @param[in] p Object to copy.
 *
 *  @return This object.
 */
perfdata& perfdata::operator=(perfdata const& p) {
  event::operator=(p);
  _internal_copy(p);
  return (*this);
}

/**
 *  Get the event type (PERFDATA).
 *
 *  @return event::PERFDATA.
 */
unsigned int perfdata::type() const {
  return (PERFDATA);
}
