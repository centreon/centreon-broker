/*
** Copyright 2011-2013 Merethis
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

#include "com/centreon/broker/notification/utilities/qhash_func.hh"
#include <QRegExp>
#include <QStringList>
#include "com/centreon/broker/logging/logging.hh"
#include "com/centreon/broker/notification/objects/string.hh"
#include "com/centreon/broker/notification/objects/command.hh"
#include "com/centreon/broker/notification/state.hh"
#include "com/centreon/broker/notification/macro_generator.hh"

using namespace com::centreon::broker::notification;
using namespace com::centreon::broker::notification::objects;

const QRegExp command::_macro_regex("\\$(\\w+)\\$");

// Forward declaration.
static void single_pass_replace(
              std::string &str,
              macro_generator::macro_container const& macros);

/**
 *  Constructor from a base command string.
 *
 *  @param[in] base_command  The command from which to construct this object.
 */
command::command(std::string const& base_command) :
  _base_command(base_command) {}

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
    _name = obj._name;
    _base_command = obj._base_command;
  }
  return (*this);
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
std::string command::resolve(
                       contact::ptr const& cnt,
                       node::ptr const& n,
                       node_cache const& cache,
                       state const& st,
                       action const& act) {
  // Match all the macros with the wonderful magic of RegExp.
  if (_macro_regex.indexIn(
        QString::fromStdString(_base_command)) == -1)
    return (_base_command);
  QStringList macro_list = _macro_regex.capturedTexts();
  macro_list.removeDuplicates();
  if (!macro_list.empty())
    macro_list.pop_front();

  // Generate each macro.
  macro_generator::macro_container macros;
  macros.reserve(macro_list.size());
  for (QStringList::const_iterator it(macro_list.begin()),
                                   end(macro_list.end());
       it != end;
       ++it)
    macros.insert(it->toStdString(), "");
  try {
    macro_generator generator;
    generator.generate(macros, n->get_node_id(), *cnt, st, cache, act);
  }
  catch (std::exception const& e) {
    logging::error(logging::medium)
      << "notification: couldn't resolve the macro '" << _name
      << "' (content: <<'" << _base_command << "'): " << e.what();
  } catch(...) {}

  // Replace the macros by their values.
  std::string resolved_command = _base_command;
  single_pass_replace(resolved_command, macros);

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
  size_t pos = 0;

  std::vector<std::pair<std::string, std::string> > macro_list;
  for (macro_generator::macro_container::iterator it(macros.begin()),
                                                  end(macros.end());
       it != end;
       ++it) {
    std::string key = it.key();
    key.append("$");
    key.insert(key.begin(), '$');
    macro_list.push_back(std::make_pair(key, *it));
  }

  while (true) {
    size_t newpos = std::string::npos;
    std::vector<std::pair<std::string, std::string> >::const_iterator
      first_resolved_macro = macro_list.end();
    for (std::vector<std::pair<std::string, std::string> >::const_iterator
           it(macro_list.begin()),
           end(macro_list.end());
         it != end;
        ++it) {
      size_t tmp = str.find(it->second, pos);
      if (tmp < newpos) {
        newpos = tmp;
        first_resolved_macro = it;
      }
    }
    if (newpos == std::string::npos)
      return ;
    str.replace(
          pos,
          first_resolved_macro->first.size(),
          first_resolved_macro->second);
    pos = newpos + first_resolved_macro->second.size();
  }
}
