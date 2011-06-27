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

#include "com/centreon/broker/storage/status.hh"

using namespace com::centreon::broker::storage;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] s Object to copy.
 */
void status::_internal_copy(status const& s) {
  ctime = s.ctime;
  index_id = s.index_id;
  interval = s.interval;
  rrd_len = s.rrd_len;
  state = s.state;
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
status::status()
  : ctime(0),
    index_id(0),
    interval(0),
    rrd_len(0),
    state(0) {}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
status::status(status const& s) : io::data(s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
status::~status() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
status& status::operator=(status const& s) {
  io::data::operator=(s);
  _internal_copy(s);
  return (*this);
}

/**
 *  Get the event type.
 *
 *  @return The string "com::centreon::broker::storage::status".
 */
QString const& status::type() const {
  static QString const status_type(
    "com::centreon::broker::storage::status");
  return (status_type);
}
