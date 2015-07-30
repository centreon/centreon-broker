/*
** Copyright 2015 Merethis
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

#ifndef TEST_BROKER_EXTCMD_HH
#  define TEST_BROKER_EXTCMD_HH

#  include <QLocalSocket>
#  include <string>

/**
 *  @class broker_extcmd broker_extcmd.hh "test/broker_extcmd.hh"
 *  @brief Interface between Broker and users.
 *
 *  Handle Centreon Broker external command file access.
 */
class                broker_extcmd {
public:
                     broker_extcmd();
                     broker_extcmd(broker_extcmd const& other);
                     ~broker_extcmd();
  broker_extcmd&     operator=(broker_extcmd const& other);
  bool               execute(
                       std::string const& query,
                       bool wait_command = true);
  std::string const& get_file() const throw ();
  void               set_file(std::string const& file);

private:
  void               _internal_copy(broker_extcmd const& other);
  void               _read(
                       QLocalSocket& sockt,
                       unsigned int& id,
                       bool& pending,
                       std::string& msg,
                       std::string const& query = std::string());
  void               _write(
                       QLocalSocket& sockt,
                       std::string const& query);

  std::string        _file;
};

#endif // !TEST_BROKER_EXTCMD_HH
