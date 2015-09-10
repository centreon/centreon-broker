/*
** Copyright 2012 Centreon
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

#ifndef TEST_EXTERNAL_COMMAND_HH
#  define TEST_EXTERNAL_COMMAND_HH

#  include <string>

/**
 *  @class external_command external_command.hh "test/external_command.hh"
 *  @brief Interface between monitoring engine and user.
 *
 *  Handle external command file access.
 */
class                external_command {
public:
                     external_command();
                     external_command(external_command const& right);
                     ~external_command();
  external_command&  operator=(external_command const& right);
  void               execute(std::string const& query);
  void               execute_broker(std::string const& query);
  std::string        get_engine_config() const;
  std::string const& get_file() const throw ();
  void               set_file(std::string const& file);

private:
  void               _internal_copy(external_command const& right);

  std::string        _file;
};

#endif // !TEST_EXTERNAL_COMMAND_HH
