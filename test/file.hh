/*
** Copyright 2012-2015 Centreon
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

#ifndef CCB_TEST_FILE_HH
#define CCB_TEST_FILE_HH

#include <map>
#include <string>
#include "com/centreon/broker/namespace.hh"

CCB_BEGIN()

namespace test {
/**
 *  Test file.
 */
class file {
 public:
  file();
  file(file const& other);
  ~file();
  file& operator=(file const& other);
  void close();
  std::string const& generate();
  void set(std::string const& variable, std::string const& value);
  void set_template(std::string const& base_file);

 private:
  void _internal_copy(file const& other);

  std::string _base_file;
  std::string _target_file;
  std::map<std::string, std::string> _variables;
};
}  // namespace test

CCB_END()

#endif  // !CCB_TEST_FILE_HH
