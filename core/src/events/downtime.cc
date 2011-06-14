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

#include "events/downtime.hh"

using namespace com::centreon::broker::events;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  @brief Copy internal data of the given object to the current object.
 *
 *  This internal method is used to copy data defined inside the
 *  downtime class from an object to the current instance. This means
 *  that no superclass data are copied. This method is used in downtime
 *  copy constructor and in the assignment operator.
 *
 *  @param[in] d Object to copy.
 *
 *  @see downtime(downtime const&)
 *  @see operator=(downtime const&)
 */
void downtime::_internal_copy(downtime const& d) {
  author = d.author;
  comment = d.comment;
  downtime_type = d.downtime_type;
  duration = d.duration;
  end_time = d.end_time;
  entry_time = d.entry_time;
  fixed = d.fixed;
  host_id = d.host_id;
  instance_id = d.instance_id;
  internal_id = d.internal_id;
  service_id = d.service_id;
  start_time = d.start_time;
  triggered_by = d.triggered_by;
  was_cancelled = d.was_cancelled;
  was_started = d.was_started;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  @brief Default constructor.
 *
 *  Set all members to their default value (0, NULL or equivalent).
 */
downtime::downtime()
  : downtime_type(0),
    duration(0),
    end_time(0),
    entry_time(0),
    fixed(false),
    host_id(0),
    instance_id(0),
    internal_id(0),
    service_id(0),
    start_time(0),
    triggered_by(0),
    was_cancelled(false),
    was_started(false) {}

/**
 *  @brief Copy constructor.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  @param[in] d Object to copy.
 */
downtime::downtime(downtime const& d) : event(d) {
  _internal_copy(d);
}

/**
 *  Destructor.
 */
downtime::~downtime() {}

/**
 *  @brief Assignment operator.
 *
 *  Copy internal data of the downtime object to the current instance.
 *
 *  @param[in] d Object to copy.
 *
 *  @return This object.
 */
downtime& downtime::operator=(downtime const& d) {
  event::operator=(d);
  _internal_copy(d);
  return (*this);
}

/**
 *  @brief Get the event's type.
 *
 *  Returns the type of this event (event::DOWNTIME). This can be useful
 *  for runtime event type identification.
 *
 *  @return event::DOWNTIME
 */
unsigned int downtime::type() const {
  return (DOWNTIME);
}
