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
downtime_map::downtime_map()
  : _actual_downtime_id(1) {

}

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
  return (*this);
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
  return (_actual_downtime_id++);
}

/**
 *  Get all the downtimes of a node.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return  A list of downtimes of this node.
 */
QList<downtime> downtime_map::get_all_downtimes_of_node(
                                node_id id) const {
  QList<downtime> ret;
  QList<unsigned int> downtime_ids = _downtime_id_by_nodes.values(id);

  for (QList<unsigned int>::const_iterator
         it = downtime_ids.begin(),
         end = downtime_ids.end();
       it != end;
       ++it)
    ret.append(_downtimes[*it]);
  return (ret);
}

/**
 *  Get all the recurring downtimes of a node.
 *
 *  @param[in] id  The id of the node.
 *
 *  @return  A list of downtimes of this node.
 */
QList<downtime> downtime_map::get_all_recurring_downtimes_of_node(
                                node_id id) const {
  QList<downtime> ret;
  QList<unsigned int> downtime_ids
    = _recurring_downtime_id_by_nodes.values(id);

  for (QList<unsigned int>::const_iterator
         it = downtime_ids.begin(),
         end = downtime_ids.end();
       it != end;
       ++it)
    ret.append(_recurring_downtimes[*it]);
  return (ret);
}

/**
 *  Delete a downtime.
 *
 *  @param[in] internal_id  The id of the downtime to remove.
 */
void downtime_map::delete_downtime(downtime const& dwn) {
  _downtimes.remove(dwn.internal_id);
  _downtime_id_by_nodes.remove(
    node_id(dwn.host_id, dwn.service_id), dwn.internal_id);
  _recurring_downtimes.remove(dwn.internal_id);
  _recurring_downtime_id_by_nodes.remove(
    node_id(dwn.host_id, dwn.service_id), dwn.internal_id);
}

/**
 *  Add a downtime.
 *
 *  @param[in] dwn  The downtime to add.
 */
void downtime_map::add_downtime(downtime const& dwn) {
  if (!dwn.is_recurring) {
    _downtimes.insert(dwn.internal_id, dwn);
    _downtime_id_by_nodes.insert(
      node_id(dwn.host_id, dwn.service_id), dwn.internal_id);
  }
  else {
    _recurring_downtimes.insert(dwn.internal_id, dwn);
    _recurring_downtime_id_by_nodes.insert(
      node_id(dwn.host_id, dwn.service_id), dwn.internal_id);
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
  QHash<unsigned int, downtime>::iterator found = _downtimes.find(internal_id);
  if (found != _downtimes.end())
    return (&*found);

  found = _recurring_downtimes.find(internal_id);
  if (found != _recurring_downtimes.end())
    return (&*found);

  return (NULL);
}

/**
 *  Is this downtime recurring?
 *
 *  @param[in] internal_id  The id of the downtime.
 *
 *  @return True or false.
 */
bool downtime_map::is_recurring(unsigned int internal_id) const {
  return (_recurring_downtimes.contains(internal_id));
}

/**
 *  Get all the recurring downtimes.
 *
 *  @return  The recurring downtimes.
 */
QList<downtime> downtime_map::get_all_recurring_downtimes() const {
  return (_recurring_downtimes.values());
}

/**
 *  Get all the downtimes.
 *
 *  @return  The downtimes.
 */
QList<downtime> downtime_map::get_all_downtimes() const {
  QList<downtime> ret = _recurring_downtimes.values();
  ret += _downtimes.values();
  return (ret);
  //return (_recurring_downtimes.values() + _downtimes.values());
}

/**
 *  Return true if a spawned downtime exist.
 *
 *  @param[in] parent_id  The id of the parent downtime.
 *
 *  @return               True if a spawned downtime exist.
 */
bool downtime_map::spawned_downtime_exist(unsigned int parent_id) const {
  for (QHash<unsigned int, neb::downtime>::const_iterator
         it = _downtimes.begin(),
         end = _downtimes.end();
       it != end;
       ++it)
    if (it->triggered_by == parent_id)
      return (true);
  return (false);
}
