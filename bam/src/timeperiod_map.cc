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

#include "com/centreon/broker/bam/timeperiod_map.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::bam;

/**
 *  Default constructor.
 */
timeperiod_map::timeperiod_map() {}

/**
 *  Destructor.
 */
timeperiod_map::~timeperiod_map() {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
timeperiod_map::timeperiod_map(timeperiod_map const& other) {
  timeperiod_map::operator=(other);
}

/**
 *  Assignment operator.
 *
 *  @param[in] other  The object to copy.
 *
 *  @return  A reference to this object.
 */
timeperiod_map& timeperiod_map::operator=(timeperiod_map const& other) {
  if (this != &other) {
    _map = other._map;
  }
  return (*this);
}

/**
 *  Comparison operator.
 *
 *  @param[in] other  The object to compare.
 *
 *  @return  True if both objects were equal.
 */
bool timeperiod_map::operator==(timeperiod_map const& other) const {
  return (_map == other._map);
}

/**
 *  Get the timeperiod associated with an id.
 *
 *  @param[in] id  The id.
 *
 *  @return  A timeperiod ptr toward the timeperiod, or a null ptr.
 */
bam::time::timeperiod::ptr timeperiod_map::get_timeperiod(unsigned int id) {
  std::map<unsigned int, bam::time::timeperiod::ptr>::iterator
      found = _map.find(id);
  if (found == _map.end())
    return (bam::time::timeperiod::ptr());
  else
    return (found->second);
}

/**
 *  Add a timeperiod to the map.
 *
 *  @param[in] id   The id of the timeperiod.
 *  @param[in] ptr  A ptr to the timeperiod to add.
 */
void timeperiod_map::add_timeperiod(
                       unsigned int id,
                       bam::time::timeperiod::ptr ptr) {
  _map[id] = ptr;
}

/**
 *  Clear the map.
 */
void timeperiod_map::clear() {
  _map.clear();
}
