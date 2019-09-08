/*
** Copyright 2011-2013 Centreon
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

#ifndef CCB_NOTIFICATION_NODE_HH
#define CCB_NOTIFICATION_NODE_HH

#include <ctime>
#include <set>
#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/objects/defines.hh"
#include "com/centreon/broker/notification/objects/node_id.hh"
#include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace notification {
namespace objects {
/**
 *  @class node node.hh "com/centreon/broker/notification/objects/node.hh"
 *  @brief A node object.
 *
 *  The object containing a node of the notification module.
 */
class node {
 public:
  DECLARE_SHARED_PTR(node);

  node();
  node(node const& obj);
  node& operator=(node const& obj);

  int get_notification_number() const throw();
  void set_notification_number(int value);
  time_t get_last_notification_time() const throw();
  void set_last_notification_time(time_t value);
  node_state get_hard_state() const throw();
  void set_hard_state(node_state value);
  node_state get_soft_state() const throw();
  void set_soft_state(node_state value);
  node_id get_node_id() const throw();
  void set_node_id(node_id) throw();
  std::set<node_id> const& get_parents() const throw();
  void add_parent(node_id id);
  void remove_parent(node_id id);
  bool has_parent() const throw();
  bool get_notifications_enabled() const throw();
  void set_notifications_enabled(bool enable) throw();
  std::string const& get_notification_timeperiod() const throw();
  void set_notification_timeperiod(std::string const& tp);
  double get_notification_interval() const throw();
  void set_notification_interval(double val) throw();

 private:
  int _notification_number;
  bool _notifications_enabled;
  double _notification_interval;
  time_t _last_notification_time;
  node_state _hard_state;
  node_state _soft_state;
  node_id _id;
  std::set<node_id> _parents;
  std::string _notification_period;
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_NODE_HH
