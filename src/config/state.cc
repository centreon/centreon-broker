/*
** Copyright 2011 Merethis
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

#include <QDir>
#include "config/state.hh"

using namespace com::centreon::broker::config;

/**************************************
*                                     *
*           Private Methods           *
*                                     *
**************************************/

/**
 *  Copy internal data members.
 *
 *  @param[in] s Object to copy.
 */
void state::_internal_copy(state const& s) {
  _inputs = s._inputs;
  _loggers = s._loggers;
  _module_dir = s._module_dir;
  _outputs = s._outputs;
  _params = s._params;
  return ;
}

/**************************************
*                                     *
*           Public Methods            *
*                                     *
**************************************/

/**
 *  Default constructor.
 */
state::state() {
  clear();
}

/**
 *  Copy constructor.
 *
 *  @param[in] s Object to copy.
 */
state::state(state const& s) {
  _internal_copy(s);
}

/**
 *  Destructor.
 */
state::~state() {}

/**
 *  Assignment operator.
 *
 *  @param[in] s Object to copy.
 *
 *  @return This object.
 */
state& state::operator=(state const& s) {
  _internal_copy(s);
  return (*this);
}

/**
 *  Reset state to default values.
 */
void state::clear() {
  _inputs.clear();
  _loggers.clear();
  _module_dir = QDir::currentPath();
  _outputs.clear();
  _params.clear();
  return ;
}

/**
 *  Get the list of inputs.
 *
 *  @return Input list.
 */
QList<endpoint>& state::inputs() {
  return (_inputs);
}

/**
 *  Get the list of inputs.
 *
 *  @return Input list.
 */
QList<endpoint> const& state::inputs() const {
  return (_inputs);
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
QList<logger>& state::loggers() {
  return (_loggers);
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
QList<logger> const& state::loggers() const {
  return (_loggers);
}

/**
 *  Get the module directory.
 *
 *  @return Module directory.
 */
QString const& state::module_directory() const {
  return (_module_dir);
}

/**
 *  Set the module directory.
 *
 *  @param[in] dir Module directory.
 */
void state::module_directory(QString const& dir) {
  _module_dir = dir;
  return ;
}

/**
 *  Get the list of outputs.
 *
 *  @return Output list.
 */
QList<endpoint>& state::outputs() {
  return (_outputs);
}

/**
 *  Get the list of outputs.
 *
 *  @return Output list.
 */
QList<endpoint> const& state::outputs() const {
  return (_outputs);
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
QMap<QString, QString>& state::params() {
  return (_params);
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
QMap<QString, QString> const& state::params() const {
  return (_params);
}
