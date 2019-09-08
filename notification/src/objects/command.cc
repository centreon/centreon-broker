/*
** Copyright 2011-2013 Centreon
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

#include "com/centreon/broker/notification/objects/command.hh"
#include <QRegExp>
#include <QStringList>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/misc/string.hh"
#include "com/centreon/broker/notification/macro_generator.hh"
#include "com/centreon/broker/notification/state.hh"
#include "com/centreon/broker/notification/utilities/qhash_func.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

const QRegExp command::_macro_regex("\\$(\\w+)\\$");

// Forward declaration.
static void single_pass_replace(std::string& str,
                                macro_generator::macro_container const& macros);

/**
 *  Constructor from a base command string.
 *
 *  @param[in] base_command  The command from which to construct this object.
 */
command::command(std::string const& base_command)
    : _enable_shell(true), _base_command(base_command) {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
command::command(command const& obj) {
  command::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return         A reference to this object.
 */
command& command::operator=(command const& obj) {
  if (this != &obj) {
    _enable_shell = obj._enable_shell;
    _name = obj._name;
    _base_command = obj._base_command;
  }
  return (*this);
}

/**
 *  Get the enable shell flag.
 *
 *  @return  True if we enable
 */
bool command::get_enable_shell() const throw() {
  return (_enable_shell);
}

/**
 *  Set the enable shell flag.
 *
 *  @param[in] val  The new value of the enable shell flag.
 */
void command::set_enable_shell(bool val) {
  _enable_shell = val;
}

/**
 *  Get the name of this command.
 *
 *  @return  The name of this command.
 */
std::string const& command::get_name() const throw() {
  return (_name);
}

/**
 *  Set the name of this command.
 *
 *  @param[in] name  The new name of this command.
 */
void command::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Resolve this command.
 *
 *  @return  A string containing the resolved command.
 */
std::string command::resolve(contact::ptr const& cnt,
                             node::ptr const& n,
                             node_cache const& cache,
                             state const& st,
                             action const& act) {
  // Match all the macros with the wonderful magic of RegExp.
  QString base_command = QString::fromStdString(_base_command);
  macro_generator::macro_container macros;
  int index = 0;
  while ((index = _macro_regex.indexIn(base_command, index)) != -1) {
    macros.insert(_macro_regex.cap(1).toStdString(), "");
    index += _macro_regex.matchedLength();
  }
  if (macros.empty())
    return (_base_command);

  logging::debug(logging::medium)
      << "notification: found " << macros.size() << " macros";

  // Generate each macro.
  try {
    macro_generator generator;
    generator.generate(macros, n->get_node_id(), *cnt, st, cache, act);
  } catch (std::exception const& e) {
    logging::error(logging::medium)
        << "notification: could not resolve some macro in command '" << _name
        << "': " << e.what();
  }

  // Replace the macros by their values.
  std::string resolved_command = _base_command;
  single_pass_replace(resolved_command, macros);

  // If it needs to be launched into a shell, wrap it into a shell command.
  // The command is correctly escaped in single quotes
  // (no secure function doing that in our libraries, unfortunately).
  // XXX: do the same for windows.
  if (_enable_shell) {
    misc::string::replace(resolved_command, "'", "'\''");
    resolved_command.insert(0, "sh -c '");
    resolved_command.append("'");
  }

  return (resolved_command);
}

/**
 *  @brief Replace all the macros of the string in a single pass.
 *
 *  Marginally faster, and don't mangle contents and values.
 *
 *  @param[in] str     The string.
 *  @param[in] macros  The macros to replace.
 */
static void single_pass_replace(
    std::string& str,
    macro_generator::macro_container const& macros) {
  std::vector<std::pair<std::string, std::string> > macro_list;
  for (macro_generator::macro_container::iterator it(macros.begin()),
       end(macros.end());
       it != end; ++it) {
    std::string key("$");
    key.append(it.key());
    key.append("$");
    macro_list.push_back(std::make_pair(key, *it));
  }

  for (std::vector<std::pair<std::string, std::string> >::const_iterator
           it(macro_list.begin()),
       end(macro_list.end());
       it != end; ++it) {
    size_t tmp(0);
    while ((tmp = str.find(it->first, tmp)) != std::string::npos) {
      str.replace(tmp, it->first.size(), it->second);
      tmp += it->second.size();
    }
  }
}
