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

#ifndef CCB_NOTIFICATION_MACRO_CONTEXT_HH
#define CCB_NOTIFICATION_MACRO_CONTEXT_HH

#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/action.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/state.hh"

CCB_BEGIN()

namespace notification {
/**
 *  @class macro_context macro_context.hh
 * "com/centreon/broker/notification/macro_context.hh"
 *  @brief This holds the context of a macro.
 *
 *  Like the id of the node being notified, the contact being notified, etc.
 */
class macro_context {
 public:
  macro_context(objects::node_id id,
                objects::contact const& cnt,
                state const& st,
                node_cache const& cache,
                action const& act);

  objects::node_id get_id() const;
  objects::contact const& get_contact() const;
  state const& get_state() const;
  node_cache const& get_cache() const;
  action const& get_action() const;

 private:
  objects::node_id _id;
  objects::contact const& _cnt;
  state const& _st;
  node_cache const& _cache;
  action const& _act;
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_MACRO_CONTEXT_HH
