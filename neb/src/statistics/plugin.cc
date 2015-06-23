/*
** Copyright 2013,2015 Merethis
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

#include "com/centreon/broker/neb/statistics/plugin.hh"

using namespace com::centreon::broker::neb::statistics;

/**
 *  Constructor.
 *
 *  @param[in] name The plugin name.
 */
plugin::plugin(std::string const& name) : _name(name) {}

/**
 *  Copy constructor.
 *
 *  @param[in] right The object to copy.
 */
plugin::plugin(plugin const& right) {
  operator=(right);
}

/**
 *  Destructor.
 */
plugin::~plugin() {}

/**
 *  Copy operator.
 *
 *  @param[in] right The object to copy.
 *
 *  @return This object.
 */
plugin& plugin::operator=(plugin const& right) {
  if (this != &right) {
    _name = right._name;
  }
  return (*this);
}

/**
 *  Get the plugin name.
 *
 *  @return The plugin name.
 */
std::string const& plugin::name() const throw () {
  return (_name);
}
