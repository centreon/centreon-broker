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

#include "com/centreon/broker/notification/objects/nodegroup.hh"

using namespace com::centreon::broker::notification::objects;

/**
 *  Default constructor.
 */
nodegroup::nodegroup() {}

/**
 *  Copy constructor.
 *
 *  @param[in] obj  The object to copy.
 */
nodegroup::nodegroup(nodegroup const& obj) {
  nodegroup::operator=(obj);
}

/**
 *  Assignment operator.
 *
 *  @param[in] obj  The object to copy.
 *
 *  @return  A reference to this object.
 */
nodegroup& nodegroup::operator=(nodegroup const& obj) {
  if (this != &obj) {
    _alias = obj._alias;
  }
  return (*this);
}

/**
 *  Get the name of this nodegroup.
 *
 *  @return  The name of this nodegroup.
 */
std::string const& nodegroup::get_name() const {
  return (_name);
}

/**
 *  Set the name of this nodegroup.
 *
 *  @param[in] name  The name of this nodegroup.
 */
void nodegroup::set_name(std::string const& name) {
  _name = name;
}

/**
 *  Get the alias of this nodegroup.
 *
 *  @return  The alias of this nodegroup.
 */
std::string const& nodegroup::get_alias() const {
  return (_alias);
}

/**
 *  Set the alias of this nodegroup.
 *
 *  @param[in] alias  The alias of this nodegroup.
 */
void nodegroup::set_alias(std::string const& alias) {
  _alias = alias;
}
