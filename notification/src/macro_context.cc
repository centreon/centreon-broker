/*
** Copyright 2011-2014 Centreon
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
macro_context::macro_context(objects::node_id id,
                             objects::contact const& cnt,
                             state const& st,
                             node_cache const& cache,
                             action const& act)
    : _id(id), _cnt(cnt), _st(st), _cache(cache), _act(act) {}

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
