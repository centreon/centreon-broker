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

#ifndef CC_MISC_ARGUMENT_HH
#define CC_MISC_ARGUMENT_HH

#include <string>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace misc {
/**
 *  @class argument argument.hh "com/centreon/misc/argument.hh"
 *  @brief Define class argument for get options.
 *
 *  This is an argument class.
 */
class argument {
 public:
  argument(std::string const& long_name = "",
           char name = '\0',
           std::string const& description = "",
           bool has_value = false,
           bool is_set = false,
           std::string const& value = "");
  argument(argument const& right);
  ~argument() throw();
  argument& operator=(argument const& right);
  bool operator==(argument const& right) const throw();
  bool operator!=(argument const& right) const throw();
  std::string const& get_description() const throw();
  bool get_is_set() const throw();
  bool get_has_value() const throw();
  std::string const& get_long_name() const throw();
  char get_name() const throw();
  std::string const& get_value() const throw();
  void set_description(std::string const& description);
  void set_is_set(bool val) throw();
  void set_has_value(bool val) throw();
  void set_long_name(std::string const& long_name);
  void set_name(char name);
  void set_value(std::string const& value);

 private:
  argument& _internal_copy(argument const& right);

  std::string _description;
  bool _is_set;
  bool _has_value;
  std::string _long_name;
  char _name;
  std::string _value;
};
}

CC_END()

#endif  // !CC_MISC_ARGUMENT_HH
