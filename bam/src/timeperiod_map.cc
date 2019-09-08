/*
** Copyright 2014 Centreon
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

#include "com/centreon/broker/bam/timeperiod_map.hh"
#include "com/centreon/broker/exceptions/msg.hh"

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
com::centreon::broker::time::timeperiod::ptr timeperiod_map::get_timeperiod(
    unsigned int id) const {
  std::map<unsigned int,
           com::centreon::broker::time::timeperiod::ptr>::const_iterator found =
      _map.find(id);
  if (found == _map.end())
    return (com::centreon::broker::time::timeperiod::ptr());
  else
    return (found->second);
}

/**
 *  Add a timeperiod to the map.
 *
 *  @param[in] id   The id of the timeperiod.
 *  @param[in] ptr  A ptr to the timeperiod to add.
 */
void timeperiod_map::add_timeperiod(unsigned int id,
                                    time::timeperiod::ptr ptr) {
  _map[id] = ptr;
}

/**
 *  Clear the map.
 */
void timeperiod_map::clear() {
  _map.clear();
  _timeperiod_relations.clear();
}

/**
 *  Add a relation between a ba and a timeperiod.
 *
 *  @param[in] ba_id          The id of the ba.
 *  @param[in] timeperiod_id  The id of the timeperiod.
 *  @param[in] is_default     True if the timeperiod is the default timeperiod.
 */
void timeperiod_map::add_relation(unsigned int ba_id,
                                  unsigned int timeperiod_id,
                                  bool is_default) {
  _timeperiod_relations.insert(
      std::make_pair(ba_id, std::make_pair(timeperiod_id, is_default)));
}

/**
 *  Get all timeperiods by a ba id.
 *
 *  @param[in] ba_id        The id of the ba.
 *
 *  @return                 A vector of timeperiods and optional boolean set to
 * true if the timeperiod is default.
 */
std::vector<std::pair<com::centreon::broker::time::timeperiod::ptr, bool> >
timeperiod_map::get_timeperiods_by_ba_id(unsigned int ba_id) const {
  std::vector<std::pair<com::centreon::broker::time::timeperiod::ptr, bool> >
      res;
  std::pair<timeperiod_relation_map::const_iterator,
            timeperiod_relation_map::const_iterator>
      found = _timeperiod_relations.equal_range(ba_id);

  for (; found.first != found.second; ++found.first) {
    unsigned int tp_id = found.first->second.first;
    bool is_default = found.first->second.second;
    time::timeperiod::ptr tp = get_timeperiod(tp_id);
    if (!tp)
      throw exceptions::msg()
          << "BAM-BI: could not find the timeperiod " << tp_id << " in cache";
    res.push_back(std::make_pair(tp, is_default));
  }

  return (res);
}
