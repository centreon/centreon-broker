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

#ifndef CCB_NOTIFICATION_ACTION_HH
#define CCB_NOTIFICATION_ACTION_HH

#include <utility>
#include <vector>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/node_cache.hh"
#include "com/centreon/broker/notification/objects/contact.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"

CCB_BEGIN()

namespace notification {
// Forward declaration.
class state;

/**
 *  @class action action.hh "com/centreon/broker/notification/action.hh"
 *  @brief Action object.
 *
 *  Represent an action requested to the notification engine.
 */
class action {
 public:
  /**
   *  The type of the action.
   */
  enum action_type {
    unknown = 0,
    notification_processing,  // This notification is yet to be processed.
    notification_attempt,     // Normal notification.
    notification_up,          // Up notification.
    notification_ack,         // Ack notification.
    notification_downtime     // Downtime notification.
  };
  action();
  action(action const& obj);
  action& operator=(action const& obj);
  bool operator==(action const& obj) const;
  bool operator<(action const& obj) const;

  action_type get_type() const throw();
  void set_type(action_type type) throw();

  action_type get_forwarded_type() const throw();
  void set_forwarded_type(action_type type) throw();

  objects::node_id get_node_id() const throw();
  void set_node_id(objects::node_id id) throw();

  unsigned int get_notification_rule_id() const throw();
  void set_notification_rule_id(unsigned int id) throw();

  unsigned int get_notification_number() const throw();
  void set_notification_number(unsigned int num) throw();

  time_t get_at() const throw();
  void set_at(time_t at) throw();

  time_t get_first_notification_time() const throw();
  void set_first_notification_time(time_t t) throw();

  void process_action(
      state& st,
      node_cache& cache,
      std::vector<std::pair<time_t, action> >& spawned_actions) const;

 private:
  action_type _act;
  action_type _forwarded_action;
  objects::node_id _id;
  unsigned int _notification_rule_id;
  unsigned int _notification_number;
  time_t _at;
  time_t _first_time_of_notification;

  void _spawn_notification_attempts(
      ::com::centreon::broker::notification::state& st,
      std::vector<std::pair<time_t, action> >& spawned_actions) const;
  void _process_notification(
      ::com::centreon::broker::notification::state& st,
      node_cache& cache,
      std::vector<std::pair<time_t, action> >& spawned_actions) const;
};
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_ACTION_HH
