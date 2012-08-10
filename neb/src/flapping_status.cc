/*
** Copyright 2009-2012 Merethis
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

#include "com/centreon/broker/neb/flapping_status.hh"

using namespace com::centreon::broker::neb;

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
flapping_status::flapping_status()
  : comment_time(0),
    event_time(0),
    event_type(0),
    flapping_type(0),
    high_threshold(0),
    host_id(0),
    internal_comment_id(0),
    low_threshold(0),
    percent_state_change(0),
    reason_type(0),
    service_id(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] fs Object to copy.
 */
flapping_status::flapping_status(flapping_status const& fs)
  : io::data(fs) {
  _internal_copy(fs);
}

/**
 *  Destructor.
 */
flapping_status::~flapping_status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] fs Object to copy.
 *
 *  @return This object.
 */
flapping_status& flapping_status::operator=(flapping_status const& fs) {
  io::data::operator=(fs);
  _internal_copy(fs);
  return (*this);
}

/**
 *  Get the type of this event.
 *
 *  @return The string "com::centreon::broker::neb::flapping_status".
 */
QString const& flapping_status::type() const {
  static QString const fs_type("com::centreon::broker::neb::flapping_status");
  return (fs_type);
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] fs Object to copy.
 */
void flapping_status::_internal_copy(flapping_status const& fs) {
  comment_time = fs.comment_time;
  event_time = fs.event_time;
  event_type = fs.event_type;
  flapping_type = fs.flapping_type;
  high_threshold = fs.high_threshold;
  host_id = fs.host_id;
  internal_comment_id = fs.internal_comment_id;
  low_threshold = fs.low_threshold;
  percent_state_change = fs.percent_state_change;
  reason_type = fs.reason_type;
  service_id = fs.service_id;
  return ;
}
