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

#ifndef TEST_ENGINE_HH
#  define TEST_ENGINE_HH

#  include <QProcess>
#  include <string>

/**
 *  @class engine engine.hh "test/engine.hh"
 *  @brief Monitoring engine.
 *
 *  Some Nagios-compatible monitoring engine such as Centreon Engine.
 */
class         engine {
public:
              engine();
              ~engine();
  void        set_config_file(std::string const& config_file);
  void        start();
  void        stop();

private:
              engine(engine const& right);
  engine&     operator=(engine const& right);

  std::string _config_file;
  QProcess    _engine;
};

#endif // !TEST_ENGINE_HH
