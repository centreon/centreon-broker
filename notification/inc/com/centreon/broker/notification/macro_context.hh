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

#ifndef CCB_NOTIFICATION_MACRO_CONTEXT_HH
#  define CCB_NOTIFICATION_MACRO_CONTEXT_HH

#  include "com/centreon/broker/namespace.hh"
#  include "com/centreon/broker/notification/objects/node_id.hh"
#  include "com/centreon/broker/notification/state.hh"
#  include "com/centreon/broker/notification/objects/contact.hh"
#  include "com/centreon/broker/notification/node_cache.hh"
#  include "com/centreon/broker/notification/action.hh"

CCB_BEGIN()

namespace             notification {
  /**
   *  @class macro_context macro_context.hh "com/centreon/broker/notification/macro_context.hh"
   *  @brief This holds the context of a macro.
   *
   *  Like the id of the node being notified, the contact being notified, etc.
   */
  class               macro_context {
  public:
                      macro_context(
                        objects::node_id id,
                        objects::contact const& cnt,
                        state const& st,
                        node_cache const& cache,
                        action const& act);

    objects::node_id  get_id() const;
    objects::contact const&
                      get_contact() const;
    state const&      get_state() const;
    node_cache const& get_cache() const;
    action const&     get_action() const;

  private:
    objects::node_id _id;
    objects::contact const& _cnt;
    state const& _st;
    node_cache const& _cache;
    action const& _act;
  };
}

CCB_END()

#endif // !CCB_NOTIFICATION_MACRO_CONTEXT_HH
