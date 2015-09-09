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

#ifndef TEST_ENGINE_EXTCMD_HH
#  define TEST_ENGINE_EXTCMD_HH

#  include <string>

/**
 *  @class engine_extcmd engine_extcmd.hh "test/engine_extcmd.hh"
 *  @brief Interface between monitoring engine and user.
 *
 *  Handle Centreon Engine external command file access.
 */
class                engine_extcmd {
public:
                     engine_extcmd();
                     engine_extcmd(engine_extcmd const& other);
                     ~engine_extcmd();
  engine_extcmd&     operator=(engine_extcmd const& other);
  void               execute(std::string const& query);
  std::string        get_engine_config() const;
  std::string const& get_file() const throw ();
  void               set_file(std::string const& file);

private:
  void               _internal_copy(engine_extcmd const& other);

  std::string        _file;
};

#endif // !TEST_ENGINE_EXTCMD_HH
