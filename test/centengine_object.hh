/*
** Copyright 2015 Centreon
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

#ifndef CCB_TEST_CENTENGINE_OBJECT_HH
#define CCB_TEST_CENTENGINE_OBJECT_HH

#include <map>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace test {
/**
 *  Centreon Engine configuration object.
 */
class centengine_object {
 public:
  enum object_type {
    command_type = 1,
    contact_type,
    host_type,
    hostdependency_type,
    hostgroup_type,
    service_type,
    servicedependency_type,
    servicegroup_type,
    timeperiod_type
  };

  centengine_object(object_type type);
  centengine_object(centengine_object const& other);
  ~centengine_object();
  centengine_object& operator=(centengine_object const& other);
  std::string get(std::string const& variable) const;
  std::map<std::string, std::string> const& get_variables() const;
  object_type get_type() const;
  void set(std::string const& variable, std::string const& value);

 private:
  object_type _type;
  std::map<std::string, std::string> _variables;
};
}  // namespace test

CCB_END()

#endif  // !CCB_TEST_CENTENGINE_OBJECT_HH
