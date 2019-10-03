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

#ifndef CCB_NOTIFICATION_NOTIFICATION_METHOD_HH
#define CCB_NOTIFICATION_NOTIFICATION_METHOD_HH

#include <string>
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/notification/action.hh"
#include "com/centreon/broker/notification/objects/defines.hh"
#include "com/centreon/broker/notification/utilities/ptr_typedef.hh"

CCB_BEGIN()

namespace notification {
namespace objects {
/**
 *  @class notification_method notification_method.hh
 * "com/centreon/broker/notification/objects/notification_method.hh"
 *  @brief The notification method object.
 *
 */
class notification_method {
 public:
  DECLARE_SHARED_PTR(notification_method);

  notification_method();
  notification_method(notification_method const& obj);
  notification_method& operator=(notification_method const& obj);

  std::string const& get_name() const throw();
  void set_name(std::string const& val);

  uint32_t get_command_id() const throw();
  void set_command_id(uint32_t id) throw();

  uint32_t get_interval() const throw();
  void set_interval(uint32_t val) throw();

  std::string const& get_status() const throw();
  void set_status(std::string const& val);

  std::string const& get_types() const throw();
  void set_types(std::string const& val);

  uint32_t get_start() const throw();
  void set_start(uint32_t val) throw();

  uint32_t get_end() const throw();
  void set_end(uint32_t val) throw();

  bool should_be_notified_for(node_state state, bool is_service) const;
  bool should_be_notified_for(action::action_type type) const;
  bool should_be_notified_when_correlated() const;

 private:
  std::string _name;
  uint32_t _command_id;
  uint32_t _interval;
  std::string _status;
  std::string _types;
  uint32_t _start;
  uint32_t _end;
};
}  // namespace objects
}  // namespace notification

CCB_END()

#endif  // !CCB_NOTIFICATION_NOTIFICATION_METHOD_HH
