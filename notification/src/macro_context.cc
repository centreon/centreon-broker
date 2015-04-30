/*
** Copyright 2011-2014 Merethis
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

#include "com/centreon/broker/notification/macro_context.hh"

using namespace com::centreon::broker::notification;

/**
 *  Macro context constructor.
 *
 *  @param[in] id     Id of the node being notified.
 *  @param[in] cnt    Contact being notified.
 *  @param[in] st     Notification engine's state.
 *  @param[in] cache  Node cache.
 *  @param[in] act    The notification action.
 */
macro_context::macro_context(
                 objects::node_id id,
                 objects::contact const& cnt,
                 state const& st,
                 node_cache const& cache,
                 action const& act)
  : _id(id),
    _cnt(cnt),
    _st(st),
    _cache(cache),
    _act(act) {}

/**
 *  Get the if of the node being notified.
 *
 *  @return  The id of the node being notified.
 */
objects::node_id macro_context::get_id() const {
  return (_id);
}

/**
 *  Get the contact being notified.
 *
 *  @return  The notified contact.
 */
objects::contact const& macro_context::get_contact() const {
  return (_cnt);
}

/**
 *  Get the state of the engine.
 *
 *  @return  The engine's state.
 */
state const& macro_context::get_state() const {
  return (_st);
}

/**
 *  Get the node cache.
 *
 *  @return  The node cache.
 */
node_cache const& macro_context::get_cache() const {
  return (_cache);
}

/**
 *  Get the notification action.
 *
 *  @return  The notification action.
 */
action const& macro_context::get_action() const {
  return (_act);
}
