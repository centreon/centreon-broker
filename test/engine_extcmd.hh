/*
** Copyright 2012,2015 Merethis
**
** This file is part of Centreon Broker.
**
** Centreon Broker is free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License version 2
** as published by the Free Software Foundation.
**
** Centreon Broker is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
** General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Centreon Broker. If not, see
** <http://www.gnu.org/licenses/>.
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
