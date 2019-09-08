/*
** Copyright 2009-2014 Centreon
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

#include "com/centreon/broker/notification/run_queue.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
run_queue::run_queue() {}

/**
 *  Copy constructor.
 *
 * @param obj  The object to be copied.
 */
run_queue::run_queue(run_queue const& obj) {
  run_queue::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param obj  The object to be copied.
 *
 *  @return  A reference to this object.
 */
run_queue& run_queue::operator=(run_queue const& obj) {
  if (this != &obj) {
    _action_set = obj._action_set;
    _rebuild_set();
  }
  return (*this);
}

/**
 *  Rebuild the run_queue caches from the set of action.
 */
void run_queue::_rebuild_set() {
  for (std::set<action>::iterator it(_action_set.begin()),
       end(_action_set.end());
       it != end; ++it) {
    _action_by_time.insert(std::make_pair(it->get_at(), &*it));
    _action_by_node.insert(std::make_pair(it->get_node_id(), &*it));
  }
}

/**
 *  Move the content of this queue to another queue.
 *
 *  @param[out] obj  The object where to move the content of this queue.
 *  @param[in] until The max time of all moved object.
 */
void run_queue::move_to_queue(run_queue& obj, time_t until) {
  for (iterator it(begin()), it_end(end());
       it != it_end && it->first <= until;) {
    obj.run(it->first, *it->second);
    iterator tmp = it;
    ++it;
    remove(*tmp->second);
  }
}

/**
 *  Add an action to the run_queue, to be runned at a specified time.
 *
 *  @param at  The specified time to run the action.
 *  @param a   The action to run.
 */
void run_queue::run(time_t at, action a) {
  a.set_at(at);

  std::pair<std::set<action>::iterator, bool> res = _action_set.insert(a);

  _action_by_node.insert(std::make_pair(a.get_node_id(), &*res.first));
  _action_by_time.insert(std::make_pair(at, &*res.first));
}

/**
 *  Get an iterator to the beginning of the run_queue.
 *
 *  @return  An iterator to the beginning of the run_queue.
 */
run_queue::iterator run_queue::begin() {
  return (_action_by_time.begin());
}

/**
 *  Get a const iterator to the beginning of the run_queue.
 *
 *  @return  A const iterator to the beginning of the run_queue.
 */
run_queue::const_iterator run_queue::begin() const {
  return (_action_by_time.begin());
}

/**
 *  Get an iterator to the end of the run_queue.
 *
 *  @return  An iterator to the end of the run_queue.
 */
run_queue::iterator run_queue::end() {
  return (_action_by_time.end());
}

/**
 *  Get a const iterator to the end of the run_queue.
 *
 *  @return  A const iterator to the end of the run_queue.
 */
run_queue::const_iterator run_queue::end() const {
  return (_action_by_time.end());
}

/**
 *  Get the time of the next action, or time_t(-1) if no action.
 *
 *  @return  The time of the next action, or time_t(-1) if no action.
 */
time_t run_queue::get_first_time() const throw() {
  if (_action_by_time.empty())
    return (time_t(-1));
  else
    return (_action_by_time.begin()->first);
}

/**
 *  Remove the next action.
 */
void run_queue::remove_first() {
  if (!_action_by_time.empty())
    remove(*_action_by_time.begin()->second);
}

void run_queue::remove(action const& a) {
  std::set<action>::iterator it = _action_set.find(a);
  if (it == _action_set.end())
    return;

  const action* ptr = &*it;

  // Erase from _action_by_node
  std::pair<std::multimap<objects::node_id, const action*>::iterator,
            std::multimap<objects::node_id, const action*>::iterator>
      node_range = _action_by_node.equal_range(ptr->get_node_id());

  for (; node_range.first != node_range.second; ++node_range.first)
    if (node_range.first->second == ptr) {
      _action_by_node.erase(node_range.first);
      break;
    }

  // Erase from _action_by_time
  std::pair<action_map::iterator, action_map::iterator> time_range =
      _action_by_time.equal_range(ptr->get_at());

  for (; time_range.first != time_range.second; ++time_range.first)
    if (time_range.first->second == ptr) {
      _action_by_time.erase(time_range.first);
      break;
    }
}

/**
 *  Get all the actions of a node.
 *
 *  @param[in] id  The node id.
 *
 *  @return        All the actions associated to this node.
 */
std::vector<const action*> run_queue::get_actions_of_node(objects::node_id id) {
  std::vector<const action*> ret;
  std::pair<std::multimap<objects::node_id, const action*>::iterator,
            std::multimap<objects::node_id, const action*>::iterator>
      node_range = _action_by_node.equal_range(id);
  for (; node_range.first != node_range.second; ++node_range.first)
    ret.push_back(node_range.first->second);
  return (ret);
}
