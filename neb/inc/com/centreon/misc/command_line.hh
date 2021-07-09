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

#ifndef CC_MISC_COMMAND_LINE_HH
#define CC_MISC_COMMAND_LINE_HH

#include <string>
#include "com/centreon/namespace.hh"

CC_BEGIN()

namespace misc {
/**
 *  @class command_line command_line.hh "com/centreon/misc/command_line.hh"
 *  @brief Provide method to split command line arguments into array.
 *
 *  Command line is a simple way to split command line arguments
 *  into array.
 */
class command_line {
 public:
  command_line();
  command_line(char const* cmdline, unsigned int size = 0);
  command_line(std::string const& cmdline);
  command_line(command_line const& right);
  ~command_line() throw();
  command_line& operator=(command_line const& right);
  bool operator==(command_line const& right) const throw();
  bool operator!=(command_line const& right) const throw();
  int get_argc() const throw();
  char** get_argv() const throw();
  void parse(char const* cmdline, unsigned int size = 0);
  void parse(std::string const& cmdline);

 private:
  void _internal_copy(command_line const& right);
  void _release();

  int _argc;
  char** _argv;
  size_t _size;
};
}

CC_END()

#endif  // !CC_MISC_COMMAND_LINE_HH
