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

#ifndef TEST_CBD_HH
#  define TEST_CBD_HH

#  include <QProcess>
#  include <string>

/**
 *  @class cbd cbd.hh "test/cbd.hh"
 *  @brief Centreon Broker daemon.
 *
 *  Centreon Broker daemon.
 */
class         cbd {
public:
              cbd();
              ~cbd();
  void        set_config_file(std::string const& config_file);
  void        start();
  void        stop();

private:
              cbd(cbd const& right);
  cbd&        operator=(cbd const& right);

  std::string _config_file;
  QProcess    _cbd;
};

#endif // !TEST_CBD_HH
