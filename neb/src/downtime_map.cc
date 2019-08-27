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

#include "com/centreon/broker/neb/downtime_map.hh"

using namespace com::centreon::broker;
using namespace com::centreon::broker::neb;

/**
 *  Default constructor.
 */
downtime_map::downtime_map() : _actual_downtime_id{1} {}

/**
 *  Copy constructor.
 *
 *  @param[in] other  The object to copy.
 */
downtime_map::downtime_map(
  downtime_map const& other)
  : _actual_downtime_id(other._actual_downtime_id),
    _downtimes(other._downtimes),
    _downtime_id_by_nodes(other._downtime_id_by_nodes),
    _recurring_downtimes(other._recurring_downtimes),
    _recurring_downtime_id_by_nodes(other._recurring_downtime_id_by_nodes) {}

/**
 *  Assignment operator.
 *
 *  @param(in] other  The object to copy.
 *
 *  @return           Reference to this object.
 */
downtime_map& downtime_map::operator=(downtime_map const& other) {
  if (this != &other) {
    _actual_downtime_id = other._actual_downtime_id;
    _downtimes = other._downtimes;
    _downtime_id_by_nodes = other._downtime_id_by_nodes;
    _recurring_downtimes = other._recurring_downtimes;
    _recurring_downtime_id_by_nodes = other._recurring_downtime_id_by_nodes;
  }
  return *this;
}

/**
 *  Destructor.
 */
downtime_map::~downtime_map() {

}

/**
 *  Get a new downtime id.
 *
 *  @return  A new downtime id.
 */
unsigned int downtime_map::get_new_downtime_id() {
  return _actual_downtime_id++;
}

/**
 *  Get all the downtimes of a node.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return  A list of downtimes of this node.
 */
std::list<downtime> downtime_map::get_all_downtimes_of_node(
                                node_id id) const {
  std::list<downtime> ret;
  auto range(_downtime_id_by_nodes.equal_range(id));

  for (std::unordered_multimap<node_id, unsigned int>::const_iterator
       it{range.first}, end{range.second}; it != end; ++it)
    ret.push_back(_downtimes.at(it->second));
  return ret;
}

/**
 *  Get all the recurring downtimes of a node.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return  A list of downtimes of this node.
 */
std::list<downtime> downtime_map::get_all_recurring_downtimes_of_node(
                                node_id id) const {
  std::list<downtime> ret;
  auto range(_recurring_downtime_id_by_nodes.equal_range(id));

  for (std::unordered_multimap<node_id, unsigned int>::const_iterator
      it{range.first}, end{range.second}; it != end; ++it)
    ret.push_back(_recurring_downtimes.at(it->second));
  return ret;
}

/**
 *  Delete a downtime.
 *
 *  @param[in] internal_id  The id of the downtime to remove.
 */
void downtime_map::delete_downtime(downtime const& dwn) {
  _downtimes.erase(dwn.internal_id);

  node_id id{dwn.host_id, dwn.service_id};
  auto range(_downtime_id_by_nodes.equal_range(id));
  for (std::unordered_multimap<node_id, unsigned int>::const_iterator
      it{range.first}, end{range.second}; it != end; ++it)
    if (it->second == dwn.internal_id) {
      _downtime_id_by_nodes.erase(it);
      break;
    }

  _recurring_downtimes.erase(dwn.internal_id);

  range = _recurring_downtime_id_by_nodes.equal_range(id);
  for (std::unordered_multimap<node_id, unsigned int>::const_iterator
      it{range.first}, end{range.second}; it != end; ++it)
    if (it->second == dwn.internal_id) {
      _recurring_downtime_id_by_nodes.erase(it);
      break;
    }
}

/**
 *  Add a downtime.
 *
 *  @param[in] dwn  The downtime to add.
 */
void downtime_map::add_downtime(downtime const& dwn) {
  if (!dwn.is_recurring) {
    _downtimes.insert({dwn.internal_id, dwn});
    _downtime_id_by_nodes.insert(
        {node_id(dwn.host_id, dwn.service_id), dwn.internal_id});
  }
  else {
    _recurring_downtimes.insert({dwn.internal_id, dwn});
    _recurring_downtime_id_by_nodes.insert(
        {node_id(dwn.host_id, dwn.service_id), dwn.internal_id});
  }

  if (_actual_downtime_id < dwn.internal_id)
    _actual_downtime_id = dwn.internal_id + 1;
}

/**
 *  Get a downtime from its internal id.
 *
 *  @param[in] internal_id  The internal id.
 *
 *  @return  Pointer to this downtime, or a null pointer.
 */
downtime* downtime_map::get_downtime(unsigned int internal_id) {
  std::unordered_map<unsigned int, downtime>::iterator found{
      _downtimes.find(internal_id)};
  if (found != _downtimes.end())
    return &found->second;

  found = _recurring_downtimes.find(internal_id);
  if (found != _recurring_downtimes.end())
    return &found->second;

  return nullptr;
}

/**
 *  Is this downtime recurring?
 *
 *  @param[in] internal_id  The id of the downtime.
 *
 *  @return True or false.
 */
bool downtime_map::is_recurring(unsigned int internal_id) const {
  std::unordered_map<unsigned int, downtime>::const_iterator
    found{_recurring_downtimes.find(internal_id)};
  return found != _recurring_downtimes.end();
}

/**
 *  Get all the recurring downtimes.
 *
 *  @return  The recurring downtimes.
 */
std::list<downtime> downtime_map::get_all_recurring_downtimes() const {
  std::list<downtime> retval;
  for (std::unordered_map<unsigned int, downtime>::const_iterator
      it{_recurring_downtimes.begin()}, end{_recurring_downtimes.end()};
      it != end; ++it)
    retval.push_back(it->second);
  return retval;
}

/**
 *  Get all the downtimes.
 *
 *  @return  The downtimes.
 */
std::list<downtime> downtime_map::get_all_downtimes() const {
  std::list<downtime> ret;
  for (std::unordered_map<unsigned int, downtime>::const_iterator
      it{_recurring_downtimes.begin()}, end{_recurring_downtimes.end()};
      it != end; ++it)
    ret.push_back(it->second);
  for (std::unordered_map<unsigned int, downtime>::const_iterator
      it{_downtimes.begin()}, end{_downtimes.end()};
      it != end; ++it)
    ret.push_back(it->second);
  return ret;
}

/**
 *  Return true if a spawned downtime exist.
 *
 *  @param[in] parent_id  The id of the parent downtime.
 *
 *  @return               True if a spawned downtime exist.
 */
bool downtime_map::spawned_downtime_exist(unsigned int parent_id) const {
  for (std::unordered_map<unsigned int, neb::downtime>::const_iterator
         it{_downtimes.begin()},
         end{_downtimes.end()};
       it != end;
       ++it)
    if (it->second.triggered_by == parent_id)
      return true;
  return false;
}
