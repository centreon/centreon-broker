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

#ifndef CC_MISC_GET_OPTIONS_HH
#define CC_MISC_GET_OPTIONS_HH

#include <map>
#include <string>
#include <vector>
#include "com/centreon/misc/argument.hh"
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace misc {
/**
 *  @class get_options get_options.hh "com/centreon/misc/get_options.hh"
 *  @brief Parse command line arguments.
 *
 *  This class provide a simple way to parse arguments with command
 *  line style.
 */
class get_options {
 public:
  get_options();
  get_options(get_options const& right);
  virtual ~get_options() throw();
  get_options& operator=(get_options const& right);
  bool operator==(get_options const& right) const throw();
  bool operator!=(get_options const& right) const throw();
  std::map<char, argument> const& get_arguments() const throw();
  argument& get_argument(char name);
  argument const& get_argument(char name) const;
  argument& get_argument(std::string const& long_name);
  argument const& get_argument(std::string const& long_name) const;
  std::vector<std::string> const& get_parameters() const throw();
  virtual std::string help() const;
  virtual std::string usage() const;
  void print_help() const;
  void print_usage() const;

 protected:
  get_options& _internal_copy(get_options const& right);
  void _parse_arguments(int argc, char** argv);
  void _parse_arguments(std::string const& command_line);
  virtual void _parse_arguments(std::vector<std::string> const& args);

  std::map<char, argument> _arguments;
  std::vector<std::string> _parameters;

 private:
  static void _array_to_vector(int argc,
                               char** argv,
                               std::vector<std::string>& args);
  static bool _split_long(std::string const& line,
                          std::string& key,
                          std::string& value);
  static bool _split_short(std::string const& line,
                           std::string& key,
                           std::string& value);
};
}  // namespace misc

CC_END()

#endif  // !CC_MISC_GET_OPTIONS_HH
