/*
** Copyright 2011-2012 Merethis
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

#include "com/centreon/broker/config/state.hh"

using namespace com::centreon::broker::config;

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
  if (this != &s)
    _internal_copy(s);
  return (*this);
}

/**
 *  Reset state to default values.
 */
void state::clear() {
  _flush_logs = true;
  _inputs.clear();
  _log_thread_id = false;
  _loggers.clear();
  _module_dir.clear();
  _module_list.clear();
  _outputs.clear();
  _params.clear();
  return ;
}

/**
 *  Set if logs must be flushed.
 *
 *  @param[in] flush true to automatically flush log files.
 */
void state::flush_logs(bool flush) throw () {
  _flush_logs = flush;
  return ;
}

/**
 *  Check if logs must be flushed.
 *
 *  @return true if logs must be automatically flushed.
 */
bool state::flush_logs() const throw () {
  return (_flush_logs);
}

/**
 *  Get the list of inputs.
 *
 *  @return Input list.
 */
QList<endpoint>& state::inputs() throw () {
  return (_inputs);
}

/**
 *  Get the list of inputs.
 *
 *  @return Input list.
 */
QList<endpoint> const& state::inputs() const throw () {
  return (_inputs);
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
QList<logger>& state::loggers() throw () {
  return (_loggers);
}

/**
 *  Set whether or not to log thread IDs.
 *
 *  @param[in] log_id true to log thread IDs.
 */
void state::log_thread_id(bool log_id) throw () {
  _log_thread_id = log_id;
  return ;
}

/**
 *  Get whether or not to log thread IDs.
 *
 *  @return true if thread IDs must be logged.
 */
bool state::log_thread_id() const throw () {
  return (_log_thread_id);
}

/**
 *  Get the logger list.
 *
 *  @return Logger list.
 */
QList<logger> const& state::loggers() const throw () {
  return (_loggers);
}

/**
 *  Get the module directory.
 *
 *  @return Module directory.
 */
QString const& state::module_directory() const throw () {
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
 *  Get the module list.
 *
 *  @return Modifiable module list.
 */
QList<QString>& state::module_list() throw () {
  return (_module_list);
}

/**
 *  Get the module list.
 *
 *  @return Const module list.
 */
QList<QString> const& state::module_list() const throw () {
  return (_module_list);
}

/**
 *  Get the list of outputs.
 *
 *  @return Output list.
 */
QList<endpoint>& state::outputs() throw () {
  return (_outputs);
}

/**
 *  Get the list of outputs.
 *
 *  @return Output list.
 */
QList<endpoint> const& state::outputs() const throw () {
  return (_outputs);
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
QMap<QString, QString>& state::params() throw () {
  return (_params);
}

/**
 *  Get the additional parameters.
 *
 *  @return Additional parameters list.
 */
QMap<QString, QString> const& state::params() const throw () {
  return (_params);
}

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
  _log_thread_id = s._log_thread_id;
  _loggers = s._loggers;
  _module_dir = s._module_dir;
  _module_list = s._module_list;
  _outputs = s._outputs;
  _params = s._params;
  return ;
}
