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

#include <cassert>
#include <cstdlib>
#include "test/engine.hh"
#include "test/vars.hh"

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
engine::engine() {}

/**
 *  Destructor.
 */
engine::~engine() {
  stop();
}

/**
 *  Set the configuration file.
 *
 *  @param[in] config_file Configuration file.
 */
void engine::set_config_file(std::string const& config_file) {
  _config_file = config_file;
  return ;
}

/**
 *  Start monitoring engine.
 */
void engine::start() {
  if (_engine.state() == QProcess::NotRunning) {
    QStringList args;
    args.push_back(_config_file.c_str());
    _engine.start(MONITORING_ENGINE, args);
    _engine.waitForStarted();
  }
  return ;
}

/**
 *  Stop monitoring engine.
 */
void engine::stop() {
  if (_engine.state() != QProcess::NotRunning) {
    _engine.terminate();
    _engine.waitForFinished(20000);
    if (_engine.state() != QProcess::NotRunning) {
      _engine.kill();
      _engine.waitForFinished(-1);
    }
  }
  return ;
}

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy constructor.
 *
 *  @param[in] right Object to copy.
 */
engine::engine(engine const& right) {
  (void)right;
  assert(!"monitoring engine is not copyable");
  abort();
}

/**
 *  Assignment operator.
 *
 *  @param[in] right Object to copy.
 *
 *  @return This object.
 */
engine& engine::operator=(engine const& right) {
  if (this != &right) {
    assert(!"monitoring engine is not copyable");
    abort();
  }
  return (*this);
}
