/*
** Copyright 2009-2014 Merethis
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
    _actions = obj._actions;
  }
  return (*this);
}

/**
 *  Add an action to the run_queue, to be runned at a specified time.
 *
 *  @param at  The specified time to run the action.
 *  @param a   The action to run.
 */
void run_queue::run(time_t at, action a) {
  _actions.insert(std::make_pair(at, a));
}

/**
 *  Get an iterator to the beginning of the run_queue.
 *
 *  @return  An iterator to the beginning of the run_queue.
 */
run_queue::iterator run_queue::begin() {
  return (_actions.begin());
}

/**
 *  Get a const iterator to the beginning of the run_queue.
 *
 *  @return  A const iterator to the beginning of the run_queue.
 */
run_queue::const_iterator run_queue::begin() const {
  return (_actions.begin());
}

/**
 *  Get an iterator to the end of the run_queue.
 *
 *  @return  An iterator to the end of the run_queue.
 */
run_queue::iterator run_queue::end() {
  return (_actions.end());
}

/**
 *  Get a const iterator to the end of the run_queue.
 *
 *  @return  A const iterator to the end of the run_queue.
 */
run_queue::const_iterator run_queue::end() const {
  return (_actions.end());
}

/**
 *  Get the time of the next action, or time_t(-1) if no action.
 *
 *  @return  The time of the next action, or time_t(-1) if no action.
 */
time_t run_queue::get_first_time() const throw() {
  if (_actions.empty())
    return (time_t(-1));
  else
    return (_actions.begin()->first);
}

/**
 *  Remove the next action.
 */
void run_queue::remove_first() {
  if (!_actions.empty())
    _actions.erase(_actions.begin());
}
