/*
** Copyright 2012 Merethis
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
