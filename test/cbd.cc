/*
** Copyright 2012-2013 Merethis
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

#include <csignal>
#include <cstdlib>
#include "test/cbd.hh"
#include "test/vars.hh"

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
cbd::cbd() {}

/**
 *  Destructor.
 */
cbd::~cbd() {
  stop();
}

/**
 *  Set the configuration file.
 *
 *  @param[in] config_file Configuration file.
 */
void cbd::set_config_file(std::string const& config_file) {
  _config_file = config_file;
  return ;
}

/**
 *  Start daemon.
 */
void cbd::start() {
  if (_cbd.state() == QProcess::NotRunning) {
    QStringList args;
    args.push_back(_config_file.c_str());
    _cbd.start(CBD_PATH, args);
    _cbd.waitForStarted();
  }
  return ;
}

/**
 *  Stop Broker daemon.
 */
void cbd::stop() {
  if (_cbd.state() != QProcess::NotRunning) {
    _cbd.terminate();
    _cbd.waitForFinished(20000);
    if (_cbd.state() != QProcess::NotRunning) {
      _cbd.kill();
      _cbd.waitForFinished(-1);
    }
  }
  return ;
}

/**
 *  Update the cbd daemon (SIGHUP).
 */
void cbd::update() {
  pid_t pid(_cbd.pid());
  if ((pid != (pid_t)0) && (pid != (pid_t)-1))
    kill(pid, SIGHUP);
  return ;
}
