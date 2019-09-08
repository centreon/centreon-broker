/*
** Copyright 2009-2012,2015 Centreon
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

#ifndef CCB_NEB_CUSTOM_VARIABLE_HH
#define CCB_NEB_CUSTOM_VARIABLE_HH

#include "com/centreon/broker/io/event_info.hh"
#include "com/centreon/broker/mapping/entry.hh"
#include "com/centreon/broker/namespace.hh"
#include "com/centreon/broker/neb/custom_variable_status.hh"

CCB_BEGIN()

namespace neb {
/**
 *  @class custom_variable custom_variable.hh
 * "com/centreon/broker/neb/custom_variable.hh"
 *  @brief Custom variable definition.
 *
 *  Nagios allows users to declare variables attached to a
 *  specific host or service.
 */
class custom_variable : public custom_variable_status {
 public:
  custom_variable();
  custom_variable(custom_variable const& other);
  ~custom_variable();
  custom_variable& operator=(custom_variable const& other);
  unsigned int type() const;
  static unsigned int static_type();

  std::string default_value;
  bool enabled;
  short var_type;

  static mapping::entry const entries[];
  static io::event_info::event_operations const operations;

 private:
  void _internal_copy(custom_variable const& other);
};
}  // namespace neb

CCB_END()

#endif  // !CCB_NEB_CUSTOM_VARIABLE_HH
